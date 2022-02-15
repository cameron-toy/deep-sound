#include "msp.h"
#include "adc.h"
#include "clock.h"
#include "settings.h"
#include "dcollect.h"
#include "fix_fft.h"
#include "buffer.h"
#include "lpf.h"
#include "bpf.h"
#include "sigdet.h"
#include <stdio.h>

uint8_t adc_ready = 0;
uint16_t adc;

buf_t buf = {.size=20, .pos=0};


/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;        // stop watchdog timer

    uint16_t i = 0;
    uint8_t collecting = 0, foundf1, foundf2, release;

    int16_t samples[BUFSIZE];
    int16_t imag[BUFSIZE];
    uint16_t mag[BUFSIZE];

    bpf_coeff_t coeffs;
    int16_t fout;

    set_DCO(FREQ_24_MHz);
    setup_adc();
    set_sample_rate(SAMPLE_RATE_HZ);
#ifdef DEBUG
    dcollect_init(); /* For data collection */
#endif
    __enable_irq();

    /* Configure onboard buttons for start/stop of data collection */
    P1->SEL0 &= ~(BIT1 | BIT4);
    P1->SEL1 &= ~(BIT1 | BIT4);
    P1->REN |= BIT1 | BIT4;
    P1->OUT |= BIT1 | BIT4;

    /* LED output (for prototype) */
    P2->DIR |= BIT2;
    P1->DIR |= BIT0;

    /* Enable external ADC VREF+ and VREF- */
#ifndef VREF_INTERNAL
    P5->SEL0 |= (BIT6 | BIT7);
    P5->SEL0 |= (BIT6 | BIT7);
#endif

#ifdef FILTER_MODE
    bpf_find_freq(&coeffs, 3000, 0.99);
#endif
    set_release_timeout(RELEASE_TIMEOUT_MS);
    //int s = 0;
    while (1) {
        /* Reset release indicator */
        if (!(P1->IN & BIT1)) {
            P2->OUT &= ~BIT2;
        }
        /* Start */
#ifdef DEBUG
        if (!(P1->IN & BIT4)) {
            if (!collecting) {
                collecting = 1;
                i = 0;
                send_ss_seq();
            }
        }
#endif
        /* A new ADC value has been read */
        if (adc_ready) {
            adc_ready = 0;

#ifdef FILTER_MODE
            fout = update_filters(adc - ADC_MID, &coeffs);
#else

#ifdef DEBUG
            if (collecting) {
#endif
                if (i < BUFSIZE) { /* Still collecting samples */
                    samples[i] = (int16_t)adc - ADC_MID; // lpfout
                    imag[i] = 0;
                    i++;
                }
                else { /* all samples collected, perform computation */
                    collecting = 0;
                    i = 0;
                    fix_fft(samples, imag, FFT_SIZE, 0);
                    compute_freq_mag(samples, imag, mag, 0, BUFSIZE);
                    foundf1 = sigdet(samples, imag, mag, FREQL);
                    foundf2 = sigdet(samples, imag, mag, FREQH);
                    release = check_state(foundf1, foundf2);
                    //P2->OUT ^= BIT2;
//                    if (s)
//                        P2->OUT |= BIT2;//~P2->OUT & BIT2;//BIT2;
//                    else
//                        P2->OUT &= ~BIT2;
//                    s = !s;
                    if (release) {
                        P2->OUT |= BIT2;
                    }
#ifdef DEBUG
                    send_samples((int16_t *)mag);
                    send_ss_seq();
#endif
                }
#ifdef DEBUG
            } /* collecting data */
#endif
#endif
        } /* adc_ready */
    }
}
