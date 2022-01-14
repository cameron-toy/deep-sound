#ifndef DCOLLECT_H
#define DCOLLECT_H

void send_samples(int16_t *samples);
void dcollect_init(void);
void send_ss_seq(void);
void send_char(uint8_t byte);
void write_str(char *str);

#endif /* DCOLLECT_H */
