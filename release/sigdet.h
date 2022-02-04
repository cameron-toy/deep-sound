#ifndef SIGDET_H_
#define SIGDET_H_

#define SIGNAL_RADIUS 5
#define NOISE_RADIUS 50
#define SIGNAL_NOISE_RATIO_MIN 0.25
#define LPF_THRESHOLD 20

typedef enum {WAIT_F1, WAIT_F2} state_t;

void compute_freq_mag(int16_t *samples, int16_t *imag);
uint8_t sigdet(int16_t *freq, uint16_t target);
uint8_t check_state(uint8_t foundf1, uint8_t foundf2);
int16_t update_filters(int16_t adc, bpf_coeff_t *coeffs);
uint16_t set_release_timeout(uint16_t freq);

#endif /* SIGDET_H_ */
