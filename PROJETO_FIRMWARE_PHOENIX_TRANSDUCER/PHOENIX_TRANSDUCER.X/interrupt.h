void InterruptInit(void);

#define DISABLE_ALL_INTERRUPTS INTCON2bits.GIE = 0
#define ENABLE_ALL_INTERRUPTS INTCON2bits.GIE = 1


