void I2c1Init(void);
void I2C2Init(void);
void I2c1InterruptHandler(void);
void I2c2InterruptHandler(void);
void I2c1WriteByte(unsigned int uiAddress, unsigned char ucData);
unsigned char I2c1ReadByte(unsigned int uiAddress);
void I2c1WriteWord(unsigned int uiAddress, unsigned int uiData);
unsigned int I2c1ReadWord(unsigned int uiAddress);
long I2c1ReadLong(unsigned int uiAddress);
void I2c1WriteLong(unsigned int uiAddress, long ulData);



