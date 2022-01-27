#include "msp.h"
#include "bpf.h"
#include <math.h>

extern int16_t fout;

void bpf_find_freq(bpf_coeff_t *coeffs, uint16_t freq, float pole_rad) {
    coeffs->a1 = 2 * pole_rad * cos(2 * M_PI * freq / 15000);
    coeffs->a2 = -(pole_rad * pole_rad);
    coeffs->b0 = 1 - pole_rad;
}

void bpf_update(bpf_coeff_t *coeffs, int16_t nextin, int16_t out0, int16_t out1) {
    fout = coeffs->b0 * nextin + coeffs->a1 * out0 + coeffs->a2 * out1;
}
