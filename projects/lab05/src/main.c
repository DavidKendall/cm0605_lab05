#include <stdbool.h>
#include <ucos_ii.h>
#include <osutils.h>
#include <bsp.h>
#include <leds.h>
#include <buttons.h>
#include <lcd.h>

enum delayConstants {ADJUST_DELAY  = 10,   MIN_DELAY = 10,  
                     DEFAULT_DELAY = 500, MAX_DELAY  = 5000};


/*
*********************************************************************************************************
*                                            APPLICATION TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_BUTTONS_PRIO                    4
#define  APP_TASK_LINK_PRIO                       6
#define  APP_TASK_CONNECT_PRIO                    8

/*
*********************************************************************************************************
*                                            APPLICATION TASK STACKS
*********************************************************************************************************
*/

#define  APP_TASK_BUTTONS_STK_SIZE              256
#define  APP_TASK_LINK_STK_SIZE                 256
#define  APP_TASK_CONNECT_STK_SIZE              256

static OS_STK appTaskButtonsStk[APP_TASK_BUTTONS_STK_SIZE];
static OS_STK appTaskLinkStk[APP_TASK_LINK_STK_SIZE];
static OS_STK appTaskConnectStk[APP_TASK_CONNECT_STK_SIZE];

/*
*********************************************************************************************************
*                                            APPLICATION TASK FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void appTaskButtons(void *pdata);
static void appTaskLinkLed(void *pdata);
static void appTaskConnectLed(void *pdata);


/*
*********************************************************************************************************
*                                           OTHER LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void incDelay(uint16_t *, uint8_t);
void decDelay(uint16_t *, uint8_t);


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLE DEFINITIONS
*********************************************************************************************************
*/

static bool flashing = false;
static uint16_t linkLedDelay = 500;
static uint16_t connectLedDelay = 500;

static OS_EVENT *lcdSem;


/*
*********************************************************************************************************
*                                            GLOBAL FUNCTION DEFINITIONS
*********************************************************************************************************
*/

int main() {

  /* Initialise the board support package */
  bspInit();
  
  /* Initialise the OS */
  OSInit();                                                   

  /* Create the tasks */
  OSTaskCreate(appTaskButtons,                               
               (void *)0,
               (OS_STK *)&appTaskButtonsStk[APP_TASK_BUTTONS_STK_SIZE - 1],
               APP_TASK_BUTTONS_PRIO);
  
  OSTaskCreate(appTaskLinkLed,                               
               (void *)0,
               (OS_STK *)&appTaskLinkStk[APP_TASK_LINK_STK_SIZE - 1],
               APP_TASK_LINK_PRIO);
  
  OSTaskCreate(appTaskConnectLed,                               
               (void *)0,
               (OS_STK *)&appTaskConnectStk[APP_TASK_CONNECT_STK_SIZE - 1],
               APP_TASK_CONNECT_PRIO);

  /* Create the semaphores */
  lcdSem = OSSemCreate(1);
  
  /* Start the OS */
  OSStart();                                                  
  
  /* Should never arrive here */ 
  return 0;      
}

/*
*********************************************************************************************************
*                                            APPLICATION TASK DEFINITIONS
*********************************************************************************************************
*/

static void appTaskButtons(void *pdata) {
  while (true) {
    if (isButtonPressed(BUT_1)) {
      flashing = true;
    } else if (isButtonPressed(BUT_2)) {
      flashing = false;
    }
    if (flashing) {
      if (isButtonPressed(JS_UP)) {
        decDelay(&linkLedDelay, ADJUST_DELAY);
      } else if (isButtonPressed(JS_DOWN)) {
        incDelay(&linkLedDelay, ADJUST_DELAY);
      } else if (isButtonPressed(JS_RIGHT)) {
        decDelay(&connectLedDelay, ADJUST_DELAY);
      } else if (isButtonPressed(JS_LEFT)) {
        incDelay(&connectLedDelay, ADJUST_DELAY);
      }
    }
    OSTimeDly(10);
  }
}

static void appTaskLinkLed(void *pdata) {
  uint8_t osStatus;
  
  /* Start the OS ticker -- must be done in the highest priority task */
  osStartTick();
  
  /* Task main loop */
  while (true) {
    if (flashing) {
      ledToggle(USB_LINK_LED);
    }
  
    OSSemPend(lcdSem, 0, &osStatus);
    lcdSetTextPos(2, 1);
    lcdWrite("(LINK) F:%s", (flashing ? "ON " : "OFF"));
    lcdSetTextPos(2, 2);
    lcdWrite("       D:%5d", linkLedDelay);
    osStatus = OSSemPost(lcdSem);
    OSTimeDly(linkLedDelay);
  }
}

static void appTaskConnectLed(void *pdata) {
  uint8_t osStatus;

  while (true) {
    OSTimeDly(connectLedDelay);
    if (flashing) {
    ledToggle(USB_CONNECT_LED);
    }
    OSSemPend(lcdSem, 0, &osStatus);
    lcdSetTextPos(2, 4);
    lcdWrite("(CNCT) F:%s",(flashing ? "ON " : "OFF"));
    lcdSetTextPos(2, 5);
    lcdWrite("       D:%5d", connectLedDelay);
    osStatus = OSSemPost(lcdSem);
    OSTimeDly(connectLedDelay);
   } 
}


static void incDelay(uint16_t *delay, uint8_t inc) {
  if (*delay <= MAX_DELAY - inc) {
    *delay += inc;
  }
  else {
    *delay = MAX_DELAY;
  }
}

static void decDelay(uint16_t *delay, uint8_t dec) {
  if (*delay >= MIN_DELAY + dec) {
    *delay -= dec;
  }
  else {
    *delay = MIN_DELAY;
  }
}
