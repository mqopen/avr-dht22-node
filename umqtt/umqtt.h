/*
 * This file is part of umqtt.
 *
 * umqtt is free softare: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Softare Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it ill be useful,
 * but WITHOUT ANY WARRANTY; ithout even the implied arranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License                                     along ith Foobar.  If not, see <http://.gnu.org/licenses/
 * */

#ifndef __UMQTT_H__
#define __UMQTT_H__

#include <stdint.h>

#define umqtt_circ_datalen(buff) \
    ((buff)->datalen)

#define umqtt_circ_is_full(buff) \
    ((buff)->length == (buff)->datalen)

#define umqtt_circ_is_empty(buff) \
    (umqtt_circ_datalen() == 0)

/** Protocol level fien at MQTT CONNECT message. */
#define UMQTT_CONNECT_PROTOCOL_LEVEL        0x04

/**
 * Connection bit flags.
 */
#define UMQTT_CONNECT_FLAG_CLEAN_SESSION    1
#define UMQTT_CONNECT_FLAG_WILL             2
#define UMQTT_CONNECT_FLAG_WILL_RETAIN      5
#define UMQTT_CONNECT_FLAG_PASSWORD         6
#define UMQTT_CONNECT_FLAG_USERNAME         7

/** UMQTT flags */
#define UMQTT_OPT_RETAIN                    0

/** Type of MQTT packets. */
enum umqtt_packet_type {
    UMQTT_CONNECT       = 1,        /**< CONNECT */
    UMQTT_CONNACK       = 2,        /**< CONNACK */
    UMQTT_PUBLISH       = 3,        /**< PUBLISH */
    UMQTT_SUBSCRIBE     = 8,        /**< SUBSCRIBE */
    UMQTT_SUBACK        = 9,        /**< SUBACK */
    UMQTT_UNSUBSCRIBE   = 10,       /**< UNSUBSCRIBE */
    UMQTT_UNSUBACK      = 11,       /**< UNSUBACK */
    UMQTT_PINGREQ       = 12,       /**< SINGREQ */
    UMQTT_PINGRESP      = 13,       /**< PINGRESP */
    UMQTT_DISCONNECT    = 14,       /**< DISCONNECT */
};

/**
 * MQTT Quality of service.
 */
enum umqtt_qos {
    UMQTT_QOS_0 = 0,        /**< At most once delivery. */
    UMQTT_QOS_1 = 1,        /**< At least once delivery. */
    UMQTT_QOS_2 = 2,        /**< Exactly once delivery. */
};

/** State of MQTT client. */
enum umqtt_client_state {
    UMQTT_STATE_INIT,
    UMQTT_STATE_CONNECTING,
    UMQTT_STATE_CONNECTED,
    UMQTT_STATE_FAILED,
};

/** MQTT buffer. */
struct umqtt_circ_buffer {
    uint8_t *start;
    uint16_t length;

    /* Private */
    uint8_t *pointer;
    int16_t datalen;
};

/** MQTT connection object. */
struct umqtt_connection {
    struct umqtt_circ_buffer txbuff;        /**< TX buffer. */
    struct umqtt_circ_buffer rxbuff;        /**< RX buffer. */

    /** Pointer to message handler function. */
    void (*message_callback)(struct umqtt_connection *, char *topic, uint8_t *data, uint16_t len);

    /* Private */
    /* ack counters - incremented on sending, decremented on ack */
    int16_t nack_publish;
    int16_t nack_subscribe;
    int16_t nack_ping;
    int16_t message_id;
    uint8_t work_buf[5];
    int16_t work_read;
    enum umqtt_client_state state;
};

/** Configuration object for connecting to MQTT broker. */
struct umqtt_connect_config {
    uint16_t keep_alive;            /** Keep alive interval setting. */
    char *client_id;                /** Client id string. */
    char *will_topic;               /** Last will message topic or NULL. */
    uint8_t *will_message;          /** Last will message. Irrelevant when will topic is NULL or zero length. */
    uint16_t will_message_len;      /** Last will message length. Irrelevant when will topic is NULL or zero length. */
    uint8_t flags;                  /** Configuration flags. */
};

/**
 * Initiate uMQTT circular buffer.
 *
 * @param buff Pointer to buffer object.
 */
void umqtt_circ_init(struct umqtt_circ_buffer *buff);

/* Return the amount of bytes left */
int16_t umqtt_circ_push(struct umqtt_circ_buffer *buff, uint8_t *data, int16_t len);

/* Returns amount of bytes popped/peeked */
int16_t umqtt_circ_pop(struct umqtt_circ_buffer *buff, uint8_t *data, int16_t len);
int16_t umqtt_circ_peek(struct umqtt_circ_buffer *buff, uint8_t *data, int16_t len);

/**
 * Initiate MQTT object.
 *
 * @param conn Connection object.
 */
void umqtt_init(struct umqtt_connection *conn);

/**
 * Connect to MQTT broker.
 *
 * @param conn Connection object.
 * @param config Connection config object.
 */
void umqtt_connect(struct umqtt_connection *conn, struct umqtt_connect_config *config);

/**
 * Subscribe to MQTT topic.
 *
 * @param conn Connection object.
 * @param topic Topic name.
 */
void umqtt_subscribe(struct umqtt_connection *conn, char *topic);

/**
 * Publish MQTT message.
 *
 * @param conn Connection object.
 * @param topic Message topic.
 * @param data Message payload.
 * @param datalen Message payload length.
 */
void umqtt_publish(struct umqtt_connection *conn, char *topic, uint8_t *data, uint16_t datalen, uint8_t flags);

/**
 * Send PINGREQ message to MQTT broker.
 *
 * @param conn Connection object.
 */
void umqtt_ping(struct umqtt_connection *conn);

/**
 * Process RX buffer.
 *
 * @param conn Connection object.
 */
void umqtt_process(struct umqtt_connection *conn);

#endif /* __UMQTT_H__ */
