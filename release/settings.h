#ifndef SETTINGS_H_
#define SETTINGS_H_

#define FFT_SIZE 10
#define BUFSIZE 1024
#define SAMPLE_RATE_HZ 50000
#define ADC_MID 3340

/* The two frequencies we are looking for (Hz) */
#define FREQL 2930
#define FREQH 3955

#define RELEASE_TIMEOUT_MS 50
#define TRANSITION_AMOUNT 8

#define BASE_CLK_FREQ 1500000

/* Compilation modes */
#undef DEBUG
#undef FILTER_MODE
#define FFT_MODE
#define VREF_INTERNAL

#endif /* SETTINGS_H_ */
