#include "msp.h"
#include "dcollect.h"

#define BASE_CLK_FREQ 1500000
#define BRS 0x00 /* 2nd modulation stage select - from datasheet */
#define BAUD_RATE 115200

static void setup_uart(uint32_t baud_rate);

void send_samples(int16_t *samples) {
    uint16_t i = 0;
    while (i < 1024) {
        send_char(samples[i] & 0xFF);
        send_char((samples[i] >> 8) & 0xFF);
        i++;
    }
}

void dcollect_init(void) {
    setup_uart(BAUD_RATE);
}

void send_ss_seq(void) {
    uint8_t i;
    for (i = 0; i < 8; i++)
        send_char(0xFF);
}

void print_adc(int16_t adc) {
//    if (adc < 0)
//        send_char('-');
//    else
//        send_char('+');
    //send_char((adc / 10000) % 10 + 48);
    send_char((adc / 1000) % 10 + 48);
    send_char((adc / 100) % 10 + 48);
    send_char((adc / 10) % 10 + 48);
    send_char((adc) % 10 + 48);
    send_char('\r');
    send_char('\n');
}

/*
 * Send a single byte via UART. Poll IFG until done.
 */
void send_char(uint8_t byte) {
    /* transfer a byte & wait until done */
    EUSCI_A0->TXBUF = byte;
    while (!(EUSCI_A0->IFG & BIT1));
}

/*
 * Write a string of characters via UART.
 * Repeatedly calls the send_char() function
 * until all chracters have been sent
 */
void write_str(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        send_char(str[i]);
        i++;
    }
}

/*
 * Configures eUSCI_A0 for UART.
 * Sets the specified baud rate and enables
 * interrupts for the receive buffer.
 * Uses P1.2 and P1.3 for TX and RX
 */
static void setup_uart(uint32_t baud_rate) {
    P1->DIR |= BIT2 | BIT3;
    P1->SEL0 |= BIT2 | BIT3; /* Set to primary function */
    P1->SEL1 &= ~(BIT3 | BIT2);

    uint32_t clk = BASE_CLK_FREQ; /* Get the clock frequency in Hz */
    clk <<= (CS->CTL0 & CS_CTL0_DCORSEL_MASK) >> CS_CTL0_DCORSEL_OFS;
    float N = clk / (float)baud_rate;
    /* BR calculation from datasheet (clock prescaler for baud rate) */
    uint16_t br = (int)(N / 16);
    /* BRF calculation from datasheet */
    uint8_t brf = (int)(((N / 16) - br) * 16);
    /* Initialization */
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST; /* Reset mode */
    /* Use SMCLK. Default LSB first, no parity, 1 stop bit */
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK;
    EUSCI_A0->BRW = br; /* Set baud rate prescaler */
    /* Enable oversampling & set BRF value */
    brf = ((brf << EUSCI_A_MCTLW_BRF_OFS) & EUSCI_A_MCTLW_BRF_MASK);
    EUSCI_A0->MCTLW |= EUSCI_A_MCTLW_OS16 | brf;
    EUSCI_A0->MCTLW |= (BRS << EUSCI_A_MCTLW_BRS_OFS) & EUSCI_A_MCTLW_BRS_MASK;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; /* Stop reset */
    /* Enable RX interrupts */
    EUSCI_A0->IE = EUSCI_A_IE_RXIE;
    NVIC->ISER[0] |= 1 << (EUSCIA0_IRQn & 0x1F);
    __enable_irq();
}
