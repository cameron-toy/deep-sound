#include "msp.h"
#include "buffer.h"
#include "cmsis_ccs.h"
#include "lpf.h"
#include <stdio.h>

int16_t lpf_update(buf_t *buf) {
    int16_t i = 0, val = 0;
    uint32_t sum = 0;
    static uint16_t bk[8] = {
        ((int16_t)(0.0026 * 8192) << 16) | (int16_t)(0.0091 * 8192), ((int16_t)(0.0222 * 8192) << 16) | (int16_t)(0.0427 * 8192),
        ((int16_t)(0.0691 * 8192) << 16) | (int16_t)(0.0971 * 8192), ((int16_t)(0.1210 * 8192) << 16) | (int16_t)(0.1347 * 8192),
        ((int16_t)(0.1347 * 8192) << 16) | (int16_t)(0.1210 * 8192), ((int16_t)(0.0971 * 8192) << 16) | (int16_t)(0.0691 * 8192),
        ((int16_t)(0.0427 * 8192) << 16) | (int16_t)(0.0222 * 8192), ((int16_t)(0.0091 * 8192) << 16) | (int16_t)(0.0026 * 8192)
    };
//    sum = __SMLAD(0x00010001, 0x00AB00BA, sum);
//    printf("sum: %8x\n", sum);
    for (i = 0; i < 8; i++) {
        //sum += bk[i] * getbuf(buf, i);
        sum = __SMLAD(bk[i], getbuf(buf, i), sum);
//        printf("bk[%d] = %d\n", i, bk[i]);
//        printf("getbuf(buf, %d) = %d\n", i, getbuf(buf, i));
//        printf("[%d] sum = %d\n", i, sum);
    }
    //__SMLALD(bk[i], getbuf(buf, ), sum);
    //i = getbuf(buf, i);
    return sum >> 13;
}

