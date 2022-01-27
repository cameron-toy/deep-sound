#include "msp.h"
#include "chirp.h"
#include "dac.h"
#include "clock.h"
#include "settings.h"
#include <stdio.h>

uint8_t dac_ready = 0;

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

	uint16_t curr_sample = 1, chirplen, chirp_table[CHIRP_TABLE_SIZE];
    int8_t sample_delta = 1;

	set_DCO(FREQ_12_MHz);
	setup_dac();

	/* At 15KHz, max chirp len is ~68000 us (if we want to fit w/in 1024 samples) */
	chirplen = create_chirp(chirp_table, 350, 1500, 65000);
	set_sample_rate(SAMPLE_RATE_HZ);

	start_chirp();

	uint16_t output = 0;

	while (1) {
	    if (dac_ready) {
	        dac_ready = 0;
	        set_DAC(chirp_table[curr_sample]);
	        //set_DAC(output);
	        //output = (output == 0 ? MAX_DAC : 0);
	        /* Sweep back and forth through chirp samples (like sawtooth) */
	        //printf("sample delta: %d\n", sample_delta);
	        curr_sample++;
	        if (curr_sample == chirplen)
	            curr_sample = 0;

	        //printf("sample %d: %d\n", curr_sample, chirp_table[curr_sample]);
	    }
	}
}
