#ifndef DAC_H_
#define DAC_H_

#define MAX_DAC 0x0FFF

void set_DAC(uint16_t data);
void setup_dac();
uint16_t set_sample_rate(uint16_t freq);

#endif /* DAC_H_ */
