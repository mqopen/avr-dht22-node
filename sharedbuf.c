#include <string.h>
#include "sharedbuf.h"

//uint8_t sharedbuf[SHAREDBUF_SIZE];

union sharedbuf_buffer sharedbuf;

void sharedbuf_clear(void) {
    memset(&sharedbuf, '\x00', sizeof(union sharedbuf_buffer));
}
