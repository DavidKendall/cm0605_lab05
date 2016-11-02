#ifndef __SAFEBUFFER_H
#define __SAFEBUFFER_H

#include <buffer.h>

#define BUF_MUTEX_PRIO  4

void safeBufferInit(void);
void safePutBuffer(message_t *);
void safeGetBuffer(message_t *);

#endif
