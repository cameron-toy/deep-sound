#include "msp.h"
#include "buffer.h"

int16_t getbuf(buf_t *buf, int16_t idx) {
    int16_t pos = buf->pos - idx;
    if (pos < 0)
        pos += buf->size;
    return (buf->data)[pos];
}

void updatebuf(buf_t *buf, int16_t val) {
    buf->pos++;
    if (buf->pos == buf->size)
        buf->pos = 0;
    buf->data[buf->pos] = val;
}


