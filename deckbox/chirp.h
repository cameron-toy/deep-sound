#ifndef CHIRP_H_
#define CHIRP_H_

uint16_t create_chirp(uint16_t *chirp_table, uint16_t freqi,
                      uint16_t freqf, uint32_t time_us);
void start_chirp(void);
void stop_chirp(void);

#endif /* CHIRP_H_ */
