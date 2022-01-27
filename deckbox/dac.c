#include "msp.h"
#include "dac.h"
#include "settings.h"

#define DAC_CTL 7 /* buffered, 1x gain */
#define THIRD_NIBBLE 0x0F00
#define LOW_BYTE 0x00FF

extern uint8_t dac_ready;

/*
 * Timer interrupt for playing samples at specific rate
 */
void TA0_0_IRQHandler(void) {
    /* Clear interrupt flag, set dac flag */
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    dac_ready = 1;
}

/*
 * Send a 12-bit number to the MCP4921
 * DAC via SPI. A total of 2 bytes are sent.
 * The first 4 bits are control bits, per the
 * MCP4921 datasheet, followed by the 12-bit
 * data, MSB first.
 */
void set_DAC(uint16_t data) {
    P5->OUT &= ~BIT0; /* CS low */
    /* transfer CTL nibble + MSB data */
    EUSCI_B0->TXBUF = (DAC_CTL << 4) | ((data & THIRD_NIBBLE) >> 8);
    while (!(EUSCI_B0->IFG & BIT1)); /* wait until done */
    /* transfer lower byte */
    EUSCI_B0->TXBUF = (data & LOW_BYTE);
    while (!(EUSCI_B0->IFG & BIT1)); /* wait until done */
    P5->OUT |= BIT0; /* CS high */
}

/*
 * Set up the SPI interface for communication with DAC
 * LDAC -> connected to GND
 * P5.0 = CS
 * P1.5 = SCK
 * P1.6 = SDI
 */
void setup_dac(void) {

    P5->DIR |= BIT0; /* CS is GPIO */
    P1->DIR |= BIT6 | BIT5;
    P1->SEL0 |= BIT6 | BIT5; /* SCK and SDI are primary function */
    P1->SEL1 &= ~(BIT6 | BIT5);

    /* Initialization */
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST; /* Reset mode */
    /* MSB first, master mode, 3-wire */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_MSB | EUSCI_B_CTLW0_MST;
    /* synchronous, use smclk */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SYNC | EUSCI_A_CTLW0_SSEL__SMCLK;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST; /* Stop reset */
}

uint16_t set_sample_rate(uint16_t freq) {
    /* Get the clock frequency in Hz and set CCR value */
    uint32_t clk = BASE_CLK_FREQ;
    clk <<= (CS->CTL0 & CS_CTL0_DCORSEL_MASK) >> CS_CTL0_DCORSEL_OFS;
    TIMER_A0->CCR[0] = (clk / freq);
    /* Enable interrupts */
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] |= 1 << (TA0_0_IRQn & 0x1F);
    /* Use SMCLK for timer */
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK;
    return 0;
}

