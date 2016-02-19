/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <avr/io.h>
#include "umqtt.h"

#define umqtt_insert_messageid(conn, ptr)   \
    do {                                    \
        ptr[0] = conn->message_id >> 8;     \
        ptr[1] = conn->message_id & 0xff;   \
        conn->message_id++;                 \
    } while (0)

#define umqtt_header_type(h) \
    ((h) >> 4)

/**
 * Create MQTT fixed header.
 *
 * @param type Packet type.
 * @param dup Duplicate delivery.
 * @param qos Quality of service.
 * @param retain retain bit.
 */
static inline uint8_t _umqtt_build_header(enum umqtt_packet_type type, uint8_t dup, uint8_t qos, uint8_t retain);

/**
 * Create remaining length field.
 *
 * @param len Length of remaining field.
 * @param data Localting of where encoded length should be stored.
 */
static uint16_t _umqtt_encode_length(int16_t len, uint8_t *data);

/**
 * Decode length field.
 */
static uint16_t umqtt_decode_length(uint8_t *data);

static void _umqtt_create_field(uint8_t *dst, uint8_t *src, uint16_t len);

void umqtt_circ_init(struct umqtt_circ_buffer *buff) {
    buff->pointer = buff->start;
    buff->datalen = 0;
}

int16_t umqtt_circ_push(struct umqtt_circ_buffer *buff, uint8_t *data, int16_t len) {
    uint8_t *bend = buff->start + buff->length - 1;
    /* This points to new byte */
    uint8_t *dend = (buff->pointer - buff->start + buff->datalen) % buff->length + buff->start;

    for (; len > 0; len--) {
        if (dend > bend)
            dend = buff->start;
        if (buff->datalen != 0 && dend == buff->pointer)
            break;
        *dend = *data;
        dend++;
        data++;
        buff->datalen++;
    }
    return len; /* Return amount of bytes left */
}

int16_t umqtt_circ_peek(struct umqtt_circ_buffer *buff, uint8_t *data, int16_t len) {
    uint8_t *ptr = buff->pointer;
    uint8_t *bend = buff->start + buff->length - 1;
    int16_t i;

    for (i = 0; i < len && i < buff->datalen; i++) {
        data[i] = ptr[i];
        if (ptr > bend)
            ptr = buff->start;
    }
    return i; /* Return the amount of bytes actually peeked */
}

int16_t umqtt_circ_pop(struct umqtt_circ_buffer *buff, uint8_t *data, int16_t len) {
    uint8_t *bend = buff->start + buff->length - 1;
    int16_t i;

    for (i = 0; i < len && buff->datalen > 0; i++) {
        data[i] = *buff->pointer;
        buff->pointer++;
        buff->datalen--;
        if (buff->pointer > bend)
            buff->pointer = buff->start;
    }
    return i; /* Return the amount of bytes actually popped */
}

void umqtt_init(struct umqtt_connection *conn) {
    conn->state = UMQTT_STATE_INIT;
    conn->nack_ping = 0;
    conn->nack_publish = 0;
    conn->nack_subscribe = 0;
    conn->message_id = 1; /* Id 0 is reserved */
}

void umqtt_connect(struct umqtt_connection *conn, struct umqtt_connect_config *config) {
    uint16_t cidlen = strlen(config->client_id);

    /* Check for non-zero client ID. */
    if (cidlen == 0)
        return;
    uint16_t will_topic_len = 0;
    if (config->will_topic != NULL)
        will_topic_len = strlen(config->will_topic);
    uint8_t fixed = _umqtt_build_header(UMQTT_CONNECT, 0, 0, 0);
    uint8_t remlen[4];

    uint8_t flags = _BV(UMQTT_CONNECT_FLAG_CLEAN_SESSION);

    /* Last will related flags. */
    if (will_topic_len > 0) {
        flags |= _BV(UMQTT_CONNECT_FLAG_WILL);
        if (config->flags & _BV(UMQTT_OPT_RETAIN))
            flags |= _BV(UMQTT_CONNECT_FLAG_WILL_RETAIN);
    }
    uint8_t variable[] = {
        /* Protocol name. */
        0,
        4,
        'M',
        'Q',
        'T',
        'T',

        /* Protocol level. */
        UMQTT_CONNECT_PROTOCOL_LEVEL,

        /* Connect flags. */
        flags,

        /* Keep alive. */
        config->keep_alive >> 8,
        config->keep_alive & 0xff,
    };
    uint16_t payload_len = 2 + cidlen;
    if (will_topic_len > 0)
        payload_len += 2 + will_topic_len + 2 + config->will_message_len;
    uint8_t payload[payload_len];

    _umqtt_create_field(payload, (uint8_t *) config->client_id, cidlen);
    if (will_topic_len > 0) {
        _umqtt_create_field(payload + 2 + cidlen, (uint8_t *) config->will_topic, will_topic_len);
        _umqtt_create_field(payload + 2 + cidlen + 2 + will_topic_len, config->will_message, config->will_message_len);
    }

    umqtt_circ_push(&conn->txbuff, &fixed, 1);
    umqtt_circ_push(&conn->txbuff, remlen, _umqtt_encode_length(sizeof(variable) + payload_len, remlen));
    umqtt_circ_push(&conn->txbuff, variable, sizeof(variable));
    umqtt_circ_push(&conn->txbuff, payload, payload_len);

    conn->state = UMQTT_STATE_CONNECTING;
}

void umqtt_subscribe(struct umqtt_connection *conn, char *topic) {
    uint16_t topiclen = strlen(topic);
    uint8_t fixed = _umqtt_build_header(UMQTT_SUBSCRIBE, 0, 1, 0);
    uint8_t remlen[4];
    uint8_t messageid[2];
    uint8_t payload[2 + topiclen + 1];

    umqtt_insert_messageid(conn, messageid);

    _umqtt_create_field(payload, (uint8_t *) topic, topiclen);
    payload[2 + topiclen] = 0; /* QoS */

    umqtt_circ_push(&conn->txbuff, &fixed, 1);
    umqtt_circ_push(&conn->txbuff, remlen, _umqtt_encode_length(sizeof(messageid) + sizeof(payload), remlen));
    umqtt_circ_push(&conn->txbuff, messageid, sizeof(messageid));
    umqtt_circ_push(&conn->txbuff, payload, sizeof(payload));

    conn->nack_subscribe++;
}

void umqtt_publish(struct umqtt_connection *conn, char *topic, uint8_t *data, uint16_t datalen, uint8_t flags) {
    uint16_t toplen = strlen(topic);
    uint8_t retain = flags & _BV(UMQTT_OPT_RETAIN) ? 1 : 0;
    uint8_t fixed = _umqtt_build_header(UMQTT_PUBLISH, 0, 0, retain);
    uint8_t remlen[4];
    uint8_t len[2];

    umqtt_circ_push(&conn->txbuff, &fixed, 1);
    umqtt_circ_push(&conn->txbuff, remlen, _umqtt_encode_length(2 + toplen + datalen, remlen));

    len[0] = toplen >> 8;
    len[1] = toplen & 0xff;
    umqtt_circ_push(&conn->txbuff, len, sizeof(len));
    umqtt_circ_push(&conn->txbuff, (uint8_t *) topic, toplen);

    umqtt_circ_push(&conn->txbuff, data, datalen);
}

void umqtt_ping(struct umqtt_connection *conn) {
    uint8_t packet[] = {
        _umqtt_build_header(UMQTT_PINGREQ, 0, 0, 0),
        0,
    };

    umqtt_circ_push(&conn->txbuff, packet, sizeof(packet));
    conn->nack_ping++;
}

static void umqtt_handle_publish(struct umqtt_connection *conn, uint8_t *data, int16_t len) {
    uint16_t toplen = (data[0] << 8) | data[1];
    char topic[toplen + 1];
    uint8_t payload[len - 2 - toplen];

    memcpy(topic, data + 2, sizeof(topic));
    topic[sizeof(topic) - 1] = 0;
    memcpy(payload, data + 2 + toplen, sizeof(payload));

    conn->message_callback(conn, topic, payload, sizeof(payload));
}

static void umqtt_packet_arrived(struct umqtt_connection *conn, uint8_t header, int16_t len) {
    uint8_t data[len];

    umqtt_circ_pop(&conn->rxbuff, data, len);
    switch (umqtt_header_type(header)) {
        case UMQTT_CONNACK:
            if (data[1] == 0x00)
                conn->state = UMQTT_STATE_CONNECTED;
            else
                conn->state = UMQTT_STATE_FAILED;
            break;
        case UMQTT_SUBACK:
            conn->nack_subscribe--;
            break;
        case UMQTT_PINGRESP:
            conn->nack_ping--;
            break;
        case UMQTT_PUBLISH:
            umqtt_handle_publish(conn, data, len);
            break;
    }
}

void umqtt_process(struct umqtt_connection *conn) {
    uint8_t buf[5];
    uint16_t i = 2;

    while (conn->rxbuff.datalen >= 2) { /* We do have the fixed header */
        umqtt_circ_pop(&conn->rxbuff, buf, 2);
        for (i = 2; buf[i - 1] & 0x80 && i < sizeof(buf); i++)
            umqtt_circ_pop(&conn->rxbuff, &buf[i], 1);
        umqtt_packet_arrived(conn, buf[0], umqtt_decode_length(&buf[1]));
    }
}

static inline uint8_t _umqtt_build_header(enum umqtt_packet_type type, uint8_t dup, uint8_t qos, uint8_t retain) {
    return (type << 4) | (dup << 3) | (qos << 1) | retain;
}

static uint16_t _umqtt_encode_length(int16_t len, uint8_t *data) {
    uint8_t digit;
    uint16_t i = 0;

    do {
        digit = len % 128;
        len /= 128;
        if (len > 0)
            digit |= 0x80;
        data[i] = digit;
        i++;
    } while (len);
    return i; /* Return the amount of bytes used */
}

static uint16_t umqtt_decode_length(uint8_t *data) {
    uint16_t mul = 1;
    uint16_t val = 0;
    uint16_t i;

    for (i = 0; i == 0 || (data[i - 1] & 0x80); i++) {
        val += (data[i] & 0x7f) * mul;
        mul *= 128;
    }
    return val;
}

static void _umqtt_create_field(uint8_t *dst, uint8_t *src, uint16_t len) {
    dst[0] = len >> 8;
    dst[1] = len & 0xff;
    memcpy(&dst[2], src, len);
}
