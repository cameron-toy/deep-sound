#include "msp.h"
#include "chirp.h"
#include "dac.h"
#include "settings.h"
#include <math.h>
#include <stdio.h>

#define US_SCALE_DOWN 0.000001
#define US_SCALE_UP 1000000
#define US_PER_SAMPLE ((1 / (double)SAMPLE_RATE_HZ) * (double)US_SCALE_UP)

/*
 * Populates a table of chirp samples according to the given parameters.
 * freqi - the initial frequency
 * freqf - the final frequency
 * time_us - the time it takes to go from freqi to freqf (in us)
 * Returns the number of samples used for the chirp, up to a maximum
 * of CHIRP_TABLE_SIZE
 */
uint16_t create_chirp(uint16_t *chirp_table, uint16_t freqi,
                      uint16_t freqf, uint32_t time_us)
{
    uint16_t i = 0;
    double curr_time_us = 0, curr_freq;
    /* Rate at which frequency changes over time (linear) */
    double rate = (freqf - freqi) / (time_us * US_SCALE_DOWN);
    while (curr_time_us < time_us && i < CHIRP_TABLE_SIZE) {
        curr_freq = rate * (curr_time_us * US_SCALE_DOWN) + freqi;
        chirp_table[i] = (MAX_DAC / 2) * sin(2 * M_PI * curr_freq *\
                                             (curr_time_us * US_SCALE_DOWN))\
                                             + (MAX_DAC / 2);
        curr_time_us += US_PER_SAMPLE;
        i++;
    }
//    double f1, f2, t;
//    for (i = 0; i < SAMPLE_RATE_HZ / freqi; i++) {
//        t = curr_time_us * US_SCALE_DOWN;
//        f1 = sin(2 * M_PI * freqi * t);
//        f2 = sin(2 * M_PI * freqf * t);
//        chirp_table[i] = (MAX_DAC / 4) * (f1 + f2) + (MAX_DAC / 2);
////        chirp_table[i] = (MAX_DAC / 2) * sin(2 * M_PI * freqi *\
////                                             (curr_time_us * US_SCALE_DOWN))\
////                                             + (MAX_DAC / 2);
//        curr_time_us += US_PER_SAMPLE;
//        //printf("[%d]: %d\n", i, chirp_table[i]);
//    }
    return i;
}

void start_chirp(void) {
    /* Start timer in UP mode */
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
}

void stop_chirp(void) {
    /* Stop timer */
    TIMER_A0->CTL &= TIMER_A_CTL_MC__STOP;
}
