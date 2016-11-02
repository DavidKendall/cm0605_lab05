#ifndef __BUFFER_H
#define __BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#define BUF_SIZE 6


typedef struct message {
  uint8_t id;
  bool flashing;
  uint16_t delay;
} message_t;

void putBuffer(message_t *);
void getBuffer(message_t *);


#endif
