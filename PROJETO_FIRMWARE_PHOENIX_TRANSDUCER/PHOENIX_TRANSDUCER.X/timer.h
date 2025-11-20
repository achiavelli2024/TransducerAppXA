void TimerInit(void);
void Timer1InterruptHandler(void);
void SoftTimerInit(void);
void TimerStart(unsigned char ucTimer);
void TimerStop(unsigned char ucTimer);
unsigned long TimerCount(unsigned char ucTimer);
void TimerClear(unsigned char ucTimer);
void TimerLoad(unsigned char ucTimer, unsigned long ulCounter, unsigned char ucState);
unsigned char TimerStatus(unsigned char ucTimer);
extern Timer32bits sTimer[TIMERS_QUANTITY];
void TimerDelay(unsigned char timer, unsigned long value);





