#include "msp.h"
#include "buffer.h"
#include <stdio.h>

inline uint32_t getbuf(buf_t *buf, int16_t idx) {
//    printf("idx: %d\n", idx);
//    printf("buffer pos: %d\n", buf->pos);
    int16_t position = buf->pos - idx - 1;
//    printf("real position: %d\n", position);
    if (position < 0)
        position += buf->size;
//    printf("adjt position: %d\n", position);
//    printf("data at pos: %d\n", (buf->data)[position]);
    return ((buf->data)[position] << 16) | (buf->data)[position - 1];
}

void updatebuf(buf_t *buf, int16_t val) {
    buf->pos++;
    if (buf->pos == buf->size)
        buf->pos = 0;
    buf->data[buf->pos] = val;
}


