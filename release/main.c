#include "msp.h"
#include "adc.h"
#include "clock.h"
#include "dcollect.h"
#include "fix_fft.h"
#include <math.h>

uint8_t adc_ready = 0;
uint16_t adc;

#define FFT_SIZE 10
#define BUFSIZE 1024
#define SAMPLE_RATE_HZ 15000

void compute_freq_mag(int16_t *samples, int16_t *imag);

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    uint16_t i = 0;
    uint8_t running = 0;

    int16_t samples[BUFSIZE];
    int16_t imag[BUFSIZE];

	set_DCO(FREQ_12_MHz);
	setup_adc();
	set_sample_rate(SAMPLE_RATE_HZ);
	dcollect_init(); /* For data collection */
	__enable_irq();

	/* Configure onboard buttons for start/stop of data collection */
    P1->SEL0 &= ~(BIT1 | BIT4);
    P1->SEL1 &= ~(BIT1 | BIT4);
    P1->REN |= BIT1 | BIT4;
    P1->OUT |= BIT1 | BIT4;

    /* Enable external ADC VREF+ and VREF- */
    P5->SEL0 |= (BIT6 | BIT7);
    P5->SEL0 |= (BIT6 | BIT7);

	while (1) {
	    /* Start */
        if (!(P1->IN & BIT4)) {
            if (!running) {
                running = 1;
                i = 0;
                send_ss_seq();
            }
        }
	    /* A new ADC value has been read */
	    if (adc_ready) {
	        adc_ready = 0;
	        if (running) {
	            if (i < BUFSIZE) { /* Still collecting samples */
	                samples[i] = (int16_t)adc - 8192;
	                imag[i] = 0;
	                i++;
	            }
	            else { /* all samples collected, perform computation */
	                running = 0;
	                i = 0;
	                fix_fft(samples, imag, FFT_SIZE, 0);
	                compute_freq_mag(samples, imag);
	                send_samples(samples);
	                send_ss_seq();
	            }
	        } /* running */
	    } /* adc_ready */
	}
}

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
}


