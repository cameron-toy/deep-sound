#include "msp.h"
#include "clock.h"

/*
 * Selects the DCOCLK as MCLK source and
 * updates the DCO frequency.
 */
void set_DCO(uint32_t freq) {
    CS->KEY = CS_KEY_VAL; /* unlock CS */
    CS->CTL0 = freq; /* Change DCO frequency to the indicated value */
    /* Select DCOCLK as MCLK source */
    CS->CTL1 |= CS_CTL1_SELM__DCOCLK | CS_CTL1_DIVM__1;
    /* SMCLK = DCOCLK */
    CS->CTL1 |= CS_CTL1_SELS__DCOCLK | CS_CTL1_DIVS__1;
    CS->KEY = 0; /* Lock CS */
}
