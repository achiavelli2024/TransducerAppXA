void AdInit(void);
void Ad1InterruptHandler(void);
void Ad2InterruptHandler(void);
void Ad1Single12BitsSample(unsigned char ucChannel);
void Ad1MultipleSample(void);
void Ad1FastSampleStart(void);
unsigned int GetAnalog(unsigned char ucChannel);
extern AnalogChannelStruct sAdChannel;
extern const unsigned char romAdFastTable[5];
