#ifndef BPF_H_
#define BPF_H_

typedef struct {
    int32_t a1;
    int32_t a2;
    int32_t b0;
} bpf_coeff_t;

void bpf_find_freq(bpf_coeff_t *coeffs, uint16_t freq, float pole_rad);
int16_t bpf_update(bpf_coeff_t *coeffs, int16_t nextin, int16_t out0, int16_t out1);

#endif /* BPF_H_ */
