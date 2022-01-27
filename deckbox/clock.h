/*
 * clock.h
 * Allows for setting the DCO frequency.
 */

#ifndef CLOCK_H
#define CLOCK_H

#define FREQ_1_5_MHz CS_CTL0_DCORSEL_0
#define FREQ_3_MHz CS_CTL0_DCORSEL_1
#define FREQ_6_MHz CS_CTL0_DCORSEL_2
#define FREQ_12_MHz CS_CTL0_DCORSEL_3
#define FREQ_24_MHz CS_CTL0_DCORSEL_4
#define FREQ_48_MHz CS_CTL0_DCORSEL_5 /* Do not use */

void set_DCO(uint32_t freq);

#endif
