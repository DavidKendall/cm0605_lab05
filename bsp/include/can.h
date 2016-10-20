/*
 * A simple CAN driver for LPC2378
 * 
 * 11-bit identifiers only; no acceptance filtering
 * 
 * DK - 16-Nov 2010
 */

#ifndef __CAN_H
#define __CAN_H

#include <stdint.h>
#include <bsp.h>

typedef struct canMessage {
  uint32_t id;
  uint32_t len;
  uint32_t dataA;
  uint32_t dataB; 
} canMessage_t;

void canInit(void);
bool canWrite(uint8_t port, canMessage_t *message);
void canRead(uint8_t port, canMessage_t *message);
bool canReady(uint8_t port);
uint32_t canStatus(uint8_t);
void canRxInterrupt(pVoidFunc_t);
  
#endif