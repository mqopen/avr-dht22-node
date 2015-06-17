#include <string.h>
#include "sharedbuf.h"

uint8_t sharedbuf[SHAREDBUF_SIZE];

void sharedbuf_clear(void) {
    memset(sharedbuf, '\x00', SHAREDBUF_SIZE);
}
