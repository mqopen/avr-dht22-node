#include <string.h>
#include "sharedbuf.h"

union sharedbuf_buffer sharedbuf;

void sharedbuf_clear(void) {
    memset(&sharedbuf, '\x00', sizeof(union sharedbuf_buffer));
}
