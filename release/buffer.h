#ifndef BUFFER_H_
#define BUFFER_H_

#define BUFFER_SIZE 20

typedef struct {
    uint16_t size;
    int16_t data[BUFFER_SIZE];
    int16_t pos;
} buf_t;

int16_t getbuf(buf_t *buf, int16_t idx);
void updatebuf(buf_t *buf, int16_t val);

#endif /* BUFFER_H_ */
