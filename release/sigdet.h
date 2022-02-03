#ifndef SIGDET_H_
#define SIGDET_H_

#define SIGNAL_RADIUS 5
#define NOISE_RADIUS 50
#define SIGNAL_NOISE_RATIO_MIN 0.25
#define LPF_THRESHOLD 20

void compute_freq_mag(int16_t *samples, int16_t *imag);
uint8_t sigdet(int16_t *freq, uint16_t target);
int16_t update_filters(int16_t adc, bpf_coeff_t *coeffs);

#endif /* SIGDET_H_ */
