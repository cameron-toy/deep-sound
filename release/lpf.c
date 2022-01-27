#include "msp.h"
#include "buffer.h"

extern int16_t fout;

void lpf_update(buf_t *buf) {
    uint8_t i = 0;
    static float bk[16] = {
        0.0026, 0.0091, 0.0222, 0.0427,
        0.0691, 0.0971, 0.1210, 0.1347,
        0.1347, 0.1210, 0.0971, 0.0691,
        0.0427, 0.0222, 0.0091, 0.0026
    };
    fout = 0;
    for (i = 0; i < 16; i++) {
        fout += (int16_t)(bk[i] * (float)(getbuf(buf, i)));
    }
}

