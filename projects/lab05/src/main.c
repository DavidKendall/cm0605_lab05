#include <stdbool.h>
#include <ucos_ii.h>
#include <osutils.h>
#include <bsp.h>
#include <leds.h>
#include <buttons.h>
#include <lcd.h>
#include <safeBuffer.h>

enum delayConstants {ADJUST_DELAY  = 10,   MIN_DELAY = 10,  
                     DEFAULT_DELAY = 500, MAX_DELAY  = 5000};


/*
*********************************************************************************************************
*                                            APPLICATION TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_BUTTONS_PRIO                    5
#define  APP_TASK_LINK_PRIO                       6
#define  APP_TASK_CONNECT_PRIO                    8
#define  APP_TASK_DISPLAY_PRIO                    10

/*
*********************************************************************************************************
*                                            APPLICATION TASK STACKS
*********************************************************************************************************
*/

#define  APP_TASK_BUTTONS_STK_SIZE              256
#define  APP_TASK_LINK_STK_SIZE                 256
#define  APP_TASK_CONNECT_STK_SIZE              256
#define  APP_TASK_DISPLAY_STK_SIZE              256

static OS_STK appTaskButtonsStk[APP_TASK_BUTTONS_STK_SIZE];
static OS_STK appTaskLinkStk[APP_TASK_LINK_STK_SIZE];
static OS_STK appTaskConnectStk[APP_TASK_CONNECT_STK_SIZE];
static OS_STK appTaskDisplayStk[APP_TASK_DISPLAY_STK_SIZE];

/*
*********************************************************************************************************
*                                            APPLICATION TASK FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void appTaskButtons(void *pdata);
static void appTaskLinkLed(void *pdata);
static void appTaskConnectLed(void *pdata);
static void appTaskDisplay(void *pdata);


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

static bool linkLedFlashing = false;
static bool connectLedFlashing = false;
static uint16_t linkLedDelay = 500;
static uint16_t connectLedDelay = 500;


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

  OSTaskCreate(appTaskDisplay,                               
               (void *)0,
               (OS_STK *)&appTaskDisplayStk[APP_TASK_DISPLAY_STK_SIZE - 1],
               APP_TASK_DISPLAY_PRIO);

  /* Initialise the message buffer */
  safeBufferInit();
  
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
  uint32_t currentState;
  uint32_t oldState;
  message_t msg;
  
  while (true) {
    currentState = buttonsRead();
    if (currentState != oldState) {
      oldState = currentState;
      if (updateButtonState(currentState, BUT_1) == B_PRESSED_RELEASED) {
        linkLedFlashing = !linkLedFlashing;
        msg.id = USB_LINK_LED;
        msg.flashing = linkLedFlashing;
        msg.delay = linkLedDelay;
        safePutBuffer(&msg);
      }
      if (updateButtonState(currentState, BUT_2) == B_PRESSED_RELEASED) {
        connectLedFlashing = !connectLedFlashing;
        msg.id = USB_CONNECT_LED;
        msg.flashing = connectLedFlashing;
        msg.delay = connectLedDelay;
        safePutBuffer(&msg);
      }
      if (linkLedFlashing) {
        msg.id = USB_LINK_LED;
        msg.flashing = linkLedFlashing;
        if (updateButtonState(currentState, JS_UP) == B_PRESSED_RELEASED) {
          decDelay(&linkLedDelay, ADJUST_DELAY);
          msg.delay = linkLedDelay;
          safePutBuffer(&msg);
        } else if (updateButtonState(currentState, JS_DOWN) == B_PRESSED_RELEASED){
          incDelay(&linkLedDelay, ADJUST_DELAY);
          msg.delay = linkLedDelay;
          safePutBuffer(&msg);
        }
      }
      if (connectLedFlashing) {
        msg.id = USB_CONNECT_LED;
        msg.flashing = connectLedFlashing;
        if (updateButtonState(currentState, JS_RIGHT) == B_PRESSED_RELEASED) {
          decDelay(&connectLedDelay, ADJUST_DELAY);
          msg.delay = connectLedDelay;
          safePutBuffer(&msg);
        } else if (updateButtonState(currentState, JS_LEFT) == B_PRESSED_RELEASED){
          incDelay(&connectLedDelay, ADJUST_DELAY);
          msg.delay = connectLedDelay;
          safePutBuffer(&msg);
        }
      }
    }
    OSTimeDly(10);
  }
}

static void appTaskLinkLed(void *pdata) {
 
  /* Start the OS ticker -- must be done in the highest priority task */
  osStartTick();
  
  /* Task main loop */
  while (true) {
    if (linkLedFlashing) {
      ledToggle(USB_LINK_LED);
    }
    OSTimeDly(linkLedDelay);
  }
}


static void appTaskConnectLed(void *pdata) {
  while (true) {
    OSTimeDly(connectLedDelay);
    if (connectLedFlashing) {
      ledToggle(USB_CONNECT_LED);
    }
  } 
}

static void appTaskDisplay(void *pdata) {
  message_t msg;

  /* Initial display */
  lcdSetTextPos(2, 1);
  lcdWrite("(LINK) F:OFF");
  lcdSetTextPos(2, 2);
  lcdWrite("       D:%04d", linkLedDelay);
  lcdSetTextPos(2, 4);
  lcdWrite("(CNCT) F:OFF");
  lcdSetTextPos(2, 5);
  lcdWrite("       D:%04d", connectLedDelay);      

  while (true) {
    safeGetBuffer(&msg);
    if (msg.id == USB_LINK_LED) {
      lcdSetTextPos(2, 1);
      lcdWrite("(LINK) F:%s", (msg.flashing ? "ON " : "OFF"));
      lcdSetTextPos(2, 2);
      lcdWrite("       D:%04d", msg.delay);
    }
    else {
      lcdSetTextPos(2, 4);
      lcdWrite("(CNCT) F:%s",(msg.flashing ? "ON " : "OFF"));
      lcdSetTextPos(2, 5);
      lcdWrite("       D:%04d", msg.delay);      
    }
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
