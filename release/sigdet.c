#include "msp.h"
#include "settings.h"
#include "bpf.h"
#include "buffer.h"
#include "lpf.h"
#include "sigdet.h"
#include <math.h>

#define TIMEOUT_CLK_HZ 9400
#define ONE_SECOND_MS 1000

extern buf_t buf;
uint8_t timeout = 0;

uint8_t check_state(uint8_t foundf1, uint8_t foundf2) {
    static state_t state = WAIT_F1;
    static uint8_t transitions = 0;
    uint8_t release = 0;
    /* Listening for the first frequency */
    if (state == WAIT_F1) {
        if (foundf1 && !foundf2) {
            state = WAIT_F2;
            transitions++;
            TIMER_A1->R = 0; /* reset timeout */
            P1->OUT |= BIT0;
        }
        else if (!foundf1) {
            state = WAIT_F1;
        }
    }
    /* Listening for the second frequency */
    else if (state == WAIT_F2) {
        if (!foundf1 && foundf2) {
            state = WAIT_F1;
            transitions++;
            TIMER_A1->R = 0; /* reset timeout */
            P1->OUT &= ~BIT0;
        }
        else if (!foundf2) {
            state = WAIT_F2;
        }
    }
    /* No transitions occurred after some time */
    if (timeout) {
        /* Back to initial state, clear all recorded transitions */
        state = WAIT_F1;
        transitions = 0;
        P1->OUT &= ~BIT0;
        /* Reset timeout */
        timeout = 0;
        TIMER_A1->R = 0;
        TIMER_A1->CTL |= TIMER_A_CTL_MC__UP;
    }
    /* Heard the correct number of transitions - release! */
    if (transitions == TRANSITION_AMOUNT) {
        release = 1;
        state = WAIT_F1;
        transitions = 0;
    }
    return release;
}

/*
 * Calculate the frequency magnitude given an array
 * of both real and imaginary parts. Modify real array
 * in place with the result.
 */
void compute_freq_mag(int16_t *real, int16_t *imag, uint16_t *result, uint16_t start, uint16_t end) {
    uint16_t i = 0;
    uint32_t val;
//    double freq;
    uint32_t freq;
    for (i = start; i < end; i++) {
        /* Made magnitude calculation faster */
        val = (real[i] << 16) | imag[i];
        val = __SMUAD(val, val);
//        freq = real[i] * real[i] + imag[i] * imag[i];
//        freq = sqrt(freq);
        freq = val >> 1;
        freq = (freq + val / freq) >> 1;
        freq = (freq + val / freq) >> 1;
        freq = (freq + val / freq) >> 1;
        freq = (freq + val / freq) >> 1;
        freq = (freq + val / freq) >> 1;
        freq = (freq + val / freq) >> 1;
        freq = (freq + val / freq) >> 1;
        freq = (freq + val / freq) >> 1;
        result[i] = freq;
    }
    result[0] = 0;
}

uint8_t sigdet(int16_t *freq, int16_t *imag, uint16_t *mag, uint16_t target) {
    uint32_t total_energy = 0, signal_energy = 0;
    uint16_t i = 0;
    int16_t start;
    start = (target * BUFSIZE / SAMPLE_RATE_HZ) - NOISE_RADIUS;
    if (start <= 0)
        start = 1;
    compute_freq_mag(freq, imag, mag, start, start + (NOISE_RADIUS << 1));
    /* Determine total energy in wide region around target */
    for (i = start; i < start + (NOISE_RADIUS << 1); i++) {
        total_energy += mag[i];
    }
    /* Determine signal energy in narrow region around target */
    for (i = start + (NOISE_RADIUS - SIGNAL_RADIUS); i < start + (NOISE_RADIUS + SIGNAL_RADIUS); i++) {
        signal_energy += mag[i];
    }
    /* Output - turn on LED for prototype */
    if ((float)signal_energy / (float)total_energy > SIGNAL_NOISE_RATIO_MIN) {
        //P2->OUT |= BIT2; /* Signal is detected */
        return 1;
    }
    //P2->OUT &= ~BIT2; /* Signal not detected */
    return 0;
}

int16_t update_filters(int16_t adc, bpf_coeff_t *coeffs) {
    static int16_t out0 = 0, out1 = 0;
    int16_t bpfout, lpfout, fabs;
    bpfout = bpf_update(coeffs, adc, out0, out1);
    fabs = bpfout + 1;
    if (bpfout < 0)
        fabs = -(bpfout + 1);
    updatebuf(&buf, fabs);
    lpfout = lpf_update(&buf);
    if (lpfout > LPF_THRESHOLD) {
        P2->OUT |= BIT2;
    }
    else {
        P2->OUT &= ~BIT2;
    }
    out1 = out0;
    out0 = bpfout;
    return lpfout;
}

void TA1_0_IRQHandler(void) {
    /* Clear interrupt flag, start adc sample */
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A1->CTL |= TIMER_A_CTL_MC__STOP;
    timeout = 1;
}

uint16_t set_release_timeout(uint16_t timeout_ms) {
    TIMER_A1->CCR[0] = (TIMEOUT_CLK_HZ * timeout_ms) / ONE_SECOND_MS;
    /* Enable interrupts */
    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] |= 1 << (TA1_0_IRQn & 0x1F);
    /* Use ACLK for timer (9.4kHz) */
    TIMER_A1->CTL = TIMER_A_CTL_SSEL__ACLK;
    /* Start timer in UP mode */
    TIMER_A1->CTL |= TIMER_A_CTL_MC__UP;
    return 0;
}
