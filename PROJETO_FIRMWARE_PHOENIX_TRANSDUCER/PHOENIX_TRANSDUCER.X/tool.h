typedef struct ToolDataStruct
{
    unsigned char Id[ID_SIZE+1];
    unsigned char SerialNumber[SERIAL_SIZE+1];
    unsigned char Model[MODEL_SIZE+1];
    unsigned char HwVersion[VERSION_SIZE+1];
    unsigned char FwVersion[VERSION_SIZE+1];
    unsigned char State;
    unsigned char CommMode;
    unsigned int LastReset;
    unsigned long Alive;
    unsigned long LastAlive;
    unsigned int Error;
    unsigned int Temperature;
    unsigned int Angle;
    unsigned int Sin;
    unsigned int Cos;
    unsigned int CosOffSet;
    unsigned int SinOffSet;
    unsigned int Torque;
    unsigned int OpenTorque;
    unsigned int TorqueCalib;
    unsigned int TorqueVref;
    unsigned int Vref;
    unsigned int Trigger;
    unsigned int ReverseEnable;
    unsigned TempAlarm:1;
}ToolData;
//
//
typedef struct ToolLedPwmStruct
{
    unsigned int TorqueOkTimeOut;
    unsigned int TorqueOkPeriod;
    unsigned int TorqueHiTimeOut;
    unsigned int TorqueHiPeriod;
    unsigned int TorqueLowTimeOut;
    unsigned int TorqueLowPeriod;
    unsigned int CwTimeOut;
    unsigned int CwPeriod;
    unsigned int CcwTimeOut;
    unsigned int CcwPeriod;
    unsigned int ManutTimeOut;
    unsigned int ManutPeriod;
}ToolLedPwm;

