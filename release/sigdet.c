#include "msp.h"
#include "settings.h"
#include "bpf.h"
#include "buffer.h"
#include "lpf.h"
#include "sigdet.h"
#include <math.h>

extern buf_t buf;

/*
 * Calculate the frequency magnitude given an array
 * of both real and imaginary parts. Modify real array
 * in place with the result.
 */
void compute_freq_mag(int16_t *real, int16_t *imag) {
    uint16_t i = 0;
    double freq;
    for (i = 0; i < BUFSIZE; i++) {
        /* TODO (maybe?): Make this more efficient */
        freq = real[i] * real[i] + imag[i] * imag[i];
        freq = sqrt(freq);
        real[i] = freq;
    }
    real[0] = 0;
}

uint8_t sigdet(int16_t *freq, uint16_t target) {
    uint32_t total_energy = 0, signal_energy = 0;
    uint16_t i = 0;
    int16_t start;
    start = (target * BUFSIZE / SAMPLE_RATE_HZ) - NOISE_RADIUS;
    if (start <= 0)
        start = 1;
    /* Determine total energy in wide region around target */
    for (i = start; i < start + (NOISE_RADIUS << 1); i++) {
        total_energy += freq[i];
    }
    /* Determine signal energy in narrow region around target */
    for (i = start + (NOISE_RADIUS - SIGNAL_RADIUS); i < start + (NOISE_RADIUS + SIGNAL_RADIUS); i++) {
        signal_energy += freq[i];
    }
    /* Output - turn on LED for prototype */
    if ((float)signal_energy / (float)total_energy > SIGNAL_NOISE_RATIO_MIN) {
        P2->OUT |= BIT2; /* Signal is detected */
        return 1;
    }
    else {
        P2->OUT &= ~BIT2; /* Signal not detected */
        return 0;
    }
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
}
