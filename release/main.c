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
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    uint16_t i = 0;
    uint8_t running = 0, found;

    int16_t samples[BUFSIZE];
    int16_t imag[BUFSIZE];

    bpf_coeff_t coeffs;
    int16_t fout;

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

    /* LED output (for prototype) */
    P2->DIR |= BIT2;

    /* Enable external ADC VREF+ and VREF- */
//    P5->SEL0 |= (BIT6 | BIT7);
//    P5->SEL0 |= (BIT6 | BIT7);

    bpf_find_freq(&coeffs, 3000, 0.99);

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

	        //fout = update_filters(adc - ADC_MID, &coeffs);

	        //if (running) {
	            if (i < BUFSIZE) { /* Still collecting samples */
	                samples[i] = (int16_t)adc - ADC_MID; // lpfout
	                imag[i] = 0;
	                i++;
	            }
	            else { /* all samples collected, perform computation */
	                running = 0;
	                i = 0;
	                fix_fft(samples, imag, FFT_SIZE, 0);
	                compute_freq_mag(samples, imag);
	                found = sigdet(samples, 1000);
	                //send_samples(samples);
	                //send_ss_seq();
	            }
	        //} /* running */

	    } /* adc_ready */
	}
}
