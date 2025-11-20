void COM1Init(unsigned int uiBaudRate);
void COM2Init(unsigned int uiBaudRate);
void COM1Disable(void);
void COM2Disable(void);

//void Uart1TxInterruptHandler(void);
//void Uart1RxInterruptHandler(void);
extern SerialTxBufferStruct sTxBuffer;
extern SerialTxBufferStruct sTxTempBuffer;

extern SerialRxBufferStruct sCOM2RxBuffer;
extern SerialTxBufferStruct sCOM2TxBuffer;

extern SerialRxBufferStruct sCOM1RxBuffer;
extern SerialTxBufferStruct sCOM1TxBuffer;

void COM2TxStart(void);
void COM1TxStart(void);
extern unsigned int uiCounter;
extern SerialRxBufferStruct sRxBuffer;
//void SlowCommTx(void);
void Uart2SlowInit(void);
void EnableLowSpeedTx(void);
void EnableHiSpeedTx(void);




