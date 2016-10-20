#include <stdbool.h>
#include <ucos_ii.h>
#include <osutils.h>
#include <bsp.h>
#include <leds.h>

/*
*********************************************************************************************************
*                                            APPLICATION TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_LINK_PRIO                       4
#define  APP_TASK_CONNECT_PRIO                    8

/*
*********************************************************************************************************
*                                            APPLICATION TASK STACKS
*********************************************************************************************************
*/

#define  APP_TASK_LINK_STK_SIZE                 256
#define  APP_TASK_CONNECT_STK_SIZE              256

static OS_STK appTaskLinkStk[APP_TASK_LINK_STK_SIZE];
static OS_STK appTaskConnectStk[APP_TASK_CONNECT_STK_SIZE];

/*
*********************************************************************************************************
*                                            APPLICATION FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void appTaskLinkLed(void *pdata);
static void appTaskConnectLed(void *pdata);


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
  OSTaskCreate(appTaskLinkLed,                               
               (void *)0,
               (OS_STK *)&appTaskLinkStk[APP_TASK_LINK_STK_SIZE - 1],
               APP_TASK_LINK_PRIO);
  
  OSTaskCreate(appTaskConnectLed,                               
               (void *)0,
               (OS_STK *)&appTaskConnectStk[APP_TASK_CONNECT_STK_SIZE - 1],
               APP_TASK_CONNECT_PRIO);

  
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

static void appTaskLinkLed(void *pdata) {
  /* Start the OS ticker -- must be done in the highest priority task */
  osStartTick();
  
  /* Task main loop */
  while (true) {
    ledToggle(USB_LINK_LED);
    OSTimeDlyHMSM(0,0,0,500);
  }
}

static void appTaskConnectLed(void *pdata) {
  while (true) {
    OSTimeDlyHMSM(0,0,0,500);
    ledToggle(USB_CONNECT_LED);
  } 
}


