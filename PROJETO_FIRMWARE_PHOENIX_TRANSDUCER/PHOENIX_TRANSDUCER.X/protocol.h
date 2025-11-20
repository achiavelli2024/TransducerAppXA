void ByteHex2Ascii(unsigned char ucByte, unsigned char *ptrBuffer);
void CrcBufferCalc(unsigned char *ptrBuffer, unsigned char ucSize);
unsigned char ByteAscii2Hex(unsigned char * ptrAscii);
void RxCmdValidation(void);
unsigned char BufferCompare(char *ucBuf1,unsigned char *ucBuf2,unsigned char ucSize);
extern unsigned char ucIdReceived;
void CmdResponse(char *ucCmd, unsigned char ucCmdSize);
unsigned int CheckSum(unsigned int *ptrBuffer, unsigned char ucSize);
void ParametersCheckSumRefresh(void);
void BlueToothCmd(unsigned char *ucCmd, unsigned char ucCmdSize);

