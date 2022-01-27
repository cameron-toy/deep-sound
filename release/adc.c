#include "msp.h"
#include "adc.h"
#include <stdio.h>

#define BASE_CLK_FREQ 1500000

extern uint16_t adc;
extern uint8_t adc_ready;

void TA0_0_IRQHandler(void) {
    /* Clear interrupt flag, start adc sample */
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    ADC14->CTL0 |= ADC14_CTL0_SC;
}

void ADC14_IRQHandler(void) {
    adc = ADC14->MEM[0]; /* Reading clears IFG */
    adc_ready = 1;
}

void setup_adc(void) {
    /* Turn on ADC14 */
    ADC14->CTL0 = ADC14_CTL0_ON;
    /* Use SMCLK, sample & hold pulse mode */
    ADC14->CTL0 |= ADC14_CTL0_SSEL__SMCLK | ADC14_CTL0_SHP;
    /* Sample cycles */
    ADC14->CTL0 |= ADC14_CTL0_SHT0__16;
    /* 14-bit resolution */
    ADC14->CTL1 = ADC14_CTL1_RES__14BIT;
    /* Channel A14 */
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_14;
    /* Use external VREF+/- */
//    ADC14->MCTL[0] |= ADC14_MCTLN_VRSEL_14;

    /* Pin 6.1 set to tertiary function (datasheet) */
    P6->SEL0 |= BIT1;
    P6->SEL1 |= BIT1;

    /* Enable interrupts */
    ADC14->IER0 = ADC14_IER0_IE0;
    NVIC->ISER[0] |= 1 << (ADC14_IRQn & 0x1F);

    /* Enable ADC */
    ADC14->CTL0 |= ADC14_CTL0_ENC;// | ADC14_CTL0_SC;
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
    printf("CCR[0]: %d\n", TIMER_A0->CCR[0]);
    printf("clk / CCR[0] (freq?): %d\n", clk / TIMER_A0->CCR[0]);
    /* Start timer in UP mode */
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
    return 0;
}

