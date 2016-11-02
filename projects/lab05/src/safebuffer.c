#include <ucos_ii.h>
#include <safebuffer.h>

static OS_EVENT *fullSlot;
static OS_EVENT *freeSlot;
static OS_EVENT *bufMutex;


void safeBufferInit( void ) {
  uint8_t osEventErr;

  fullSlot = OSSemCreate(0);
  freeSlot = OSSemCreate(BUF_SIZE);
  bufMutex = OSMutexCreate(BUF_MUTEX_PRIO, &osEventErr);
}

void safePutBuffer(message_t *msg) {
  uint8_t osStatus;

  OSSemPend(freeSlot, 0, &osStatus);
  OSMutexPend(bufMutex, 0, &osStatus);
  putBuffer(msg);
  osStatus = OSMutexPost(bufMutex);
  osStatus = OSSemPost(fullSlot);
}


void safeGetBuffer(message_t *msg) {
  uint8_t osStatus;
  
  OSSemPend(fullSlot, 0, &osStatus);
  OSMutexPend(bufMutex, 0, &osStatus);
  getBuffer(msg);
  osStatus = OSMutexPost(bufMutex);
  osStatus = OSSemPost(freeSlot);
}

