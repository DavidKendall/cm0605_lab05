#include <stdint.h>
#include <bsp.h>

typedef enum timerIdentifier {TIMER0, TIMER1} timerIdentifier_t;

void initTimer(timerIdentifier_t timer, pVoidFunc_t handler, uint32_t ticksPerSec);
void startTimer(timerIdentifier_t timer);
void stopTimer(timerIdentifier_t timer);
