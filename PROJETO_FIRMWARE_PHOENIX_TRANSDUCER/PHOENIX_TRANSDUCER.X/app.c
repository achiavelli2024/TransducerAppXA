#include <stdio.h>
#include <stdlib.h>
#include <libq.h>
#include "hardware.h"
#include "io.h"
#include "ad.h"
#include "uart.h"
#include "timer.h"
#include "oscillator.h"
#include "crc.h"
#include "tool.h"
#include "interrupt.h"
#include "pwm.h"
#include "i2c.h"
#include "protocol.h"
#include "spi.h"
#include "power.h"
#include "ads1271.h"
#include "encoder.h"
#include "ds18s20.h"
#include "esp8266.h"
#include "peakfinder.h"
#include "butterworth.h"
//
#define TORQUE_BUFFER_INIT      0x34C0 //0x33C0  + 256 bytes para aumentar o espa�o de stack no debug
#define CONST_DATA_INIT         0x0300

extern unsigned char ucTransparentEscape;
extern unsigned char Trq0, Trq1, Trq2; 

DeviceStruct sDevice, sTempDevice;
PwrButtonStruct sPwrButton;

__attribute__((address(LOG_DATA_BLOCK1_ADDR)))unsigned char ucLogData1[LOG_DATA_BLOCK1_SIZE][5];//RAM_BUF_SIZE
__attribute__((address(LOG_DATA_BLOCK0_ADDR)))unsigned char ucLogData[LOG_DATA_BLOCK0_SIZE][5];//RAM_BUF_SIZE 0x82b4

BatVoltageStruct sBatVoltage;
extern MovAverageStruct sMovAverage;
DerivativeStruct sDerivativeTorque;
ClickStruct sTorqueClick;
unsigned char *ucPtr;
unsigned char ucCrcResult;
unsigned char ucTempVar;
unsigned char ucDelay;
unsigned int uiTempVar;
long lTempVar;
long lTemp, lTempA;
unsigned char ucTemp;
long *plong;

unsigned char ucTempError;
//
__attribute__((address(CONST_DATA_INIT-8)))const unsigned char romFwId0[FIRMWARE_ID_SIZE] = "01234567";
__attribute__((address(CONST_DATA_INIT)))const unsigned char romFwId[FIRMWARE_ID_SIZE] = FIRMWARE_ID;
__attribute__((address(CONST_DATA_INIT+FIRMWARE_ID_SIZE)))const unsigned char romFwVersion[VERSION_SIZE] = FIRMWARE_VERSION;
__attribute__((address(CONST_DATA_INIT+FIRMWARE_ID_SIZE+VERSION_SIZE)))const unsigned char romHwVersion[VERSION_SIZE] = HARDWARE_VERSION;
//
const unsigned char romSerialNumber[SERIAL_SIZE+1] = SERIAL_NUMBER;
const unsigned char romModel[MODEL_SIZE+1] = MODEL;
const unsigned char romCapacity[CAPACITY_SIZE+1] = CAPACITY;
const unsigned char romType[TYPE_SIZE+1] = TYPE;
const unsigned char romId[ID_SIZE+1] = ID;
//
unsigned char ucIndex;
long lTempTorque, lTempAngle;
long long l64Temp = 0;

unsigned int uiAdSoftStartOn = 0;
unsigned int uiAdSoftStartOff = 0;
unsigned char ucAdSoftStartCounter = 0;
unsigned char ucAdSoftStartState = 0;
//
unsigned char ucIdAscii[12];
unsigned char ucIdAsciiAddress[12];
unsigned char ucBuffer[5];
//
void HandleData(void);
void ParametersInit(void);
unsigned char StoreData(void);
void AnalogSample(void);
void PeakDetect(void);
unsigned char PwrButtonCheck(unsigned char ucData);
void PowerStatus(void);
void PeakDetectFreeTimer(void);
void IncCycleCounter(void);
void CalcOverTorqueAdValue(void);
void IncMaxTorqueCounter(void);
void TorqueDerivative(long Samplevalue);
void TorqueClickDetector(void);
//
void GenericCurve(void);
void ClickCurve(void);
void ShutOffCurve(void);
long TorqueRegistry2Long(unsigned int RegistryAddress);
void GetId(void);
void GetAddress(void);
void ComInit(void);
void HC06Config(void);
void LedRefresh(void);
void CheckComMode(void);
//
void AppRun(void)
{
    OscInit();                          //Inicializa o Oscilador e PLL
    IoInit();                           //Inicializa os IO's Digitais
    GetAddress();                       //Obtém o endereço 
    InterruptInit();                    //Inicializa os serviços de interrupção
    SoftTimerInit();                    //Inicializa os timers de software
    TimerInit();                        //Inicializa os timers
    LED_GREEN_PIN = _ON;
    GetId();                            //Obtém o ID da placa
    PowerSupplyInit();                  //Liga as fontes de alimentação
    TimerDelay(DELAYTIMER, 200);
    CrcInit();                          //Inicializa o módulo CRC
    ButterInit();
    ENABLE_ALL_INTERRUPTS;              //Habilita interrupções
    TimerDelay(DELAYTIMER, 100);
    AdcIoInit();
    TimerDelay(DELAYTIMER, 100);
    AdInit();                           //Inicializa o canal analógico
    ADS1271Init();                      //Inicializa ADS1271
    TimerDelay(DELAYTIMER, 100);
    ComInit();
    I2c1Init();
    EncoderInit();
    ParametersInit();
    TimerDelay(DELAYTIMER, 100); 
    TimerLoad(TIMEOUT_TIMER,50,RUN);
    TimerLoad(LED_TIMER,500,RUN);
    uiTempVar = 1;
    DS18S20StartConversion();
    HC06Config();
    sDevice.AcqTrigger = 0;
    TimerLoad(ACQ_TIMER,sDevice.AcquisitionTimeStep,RUN);
    //
    PeakFinderInit();
    //Main Loop
    while(1)
    {
        ClrWdt(); //Reset do C�o de guarda
        HandleData();
        PeakDetectFreeTimer();
        PowerStatus();
        if(sCOM2RxBuffer.Status == SLOWCOMM_JOB || sCOM1RxBuffer.Status == SLOWCOMM_JOB)
            RxCmdValidation();
        PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
        #if PWR_BUTTON_ENABLE == 1                                                
            PowerButtonScan(); //Verifica estado do Power Button
        #endif
        LedRefresh();
        CheckComMode();
    }
}
//
void HandleData(void)
{
    sDevice.FastScan = 0;
    if(sDevice.AcqTrigger == 1 || sDevice.FastScan == 1)
    {
        sDevice.AcqTrigger = 0;
        switch(sDevice.ToolType)
        {
            case TIPO_TORQUIMETRO_ESTALO: ClickCurve(); break;
            case TIPO_APERTADEIRA_CABO: GenericCurve(); break;
            case TIPO_APERTADEIRA_IMPULSO: GenericCurve(); break;
            case TIPO_TORQUIMETRO_DIGITAL: GenericCurve(); break;
            case TIPO_APERTADEIRA_PENUMATICA: GenericCurve(); break;
            case TIPO_APERTADEIRA_BATERIA: GenericCurve(); break;
            case TIPO_APERTADEIRA_BATERIA_TRANSD: GenericCurve(); break;
            case TIPO_APERTADEIRA_SHUT_OFF: GenericCurve(); break;
        }
    }
    if(TimerStatus(ACQ_TIMER)==OVERFLOW && (sDevice.AcquisitionStatus == ACQ_THR || sDevice.AcquisitionStatus == ACQ_FINALTIMEOUT
           || sDevice.AcquisitionStatus == ACQ_RESET_TIME || sDevice.AcquisitionStatus == ACQ_DELAY_TIME))
    {
        TimerLoad(ACQ_TIMER,sDevice.AcquisitionTimeStep,RUN);
        if(StoreData() == 0)
        {   
            //Aponta para o pr�ximo registro
            sDevice.AcquisitionIndex++;
        }
    }
}
//
unsigned char StoreData(void)
{
    //Armazena Torque e ângulo no buffer de RAM na posição apontada por sDevice.AcquisitionIndex
    //Retorna 0 se sucesso
    if(sDevice.AcquisitionIndex < LOG_DATA_TOTAL_SIZE)
    {
        //Há espaço disponível para armazenar o registro
        if(sDevice.AcquisitionIndex < LOG_DATA_BLOCK0_SIZE)
        {
            ucLogData[sDevice.AcquisitionIndex][0] = (unsigned char)(lTempTorque >> 16);
            ucLogData[sDevice.AcquisitionIndex][1] = (unsigned char)(lTempTorque >> 8);
            ucLogData[sDevice.AcquisitionIndex][2] = (unsigned char)lTempTorque;
            ucLogData[sDevice.AcquisitionIndex][3] = (unsigned char)(lTempAngle >> 8);
            ucLogData[sDevice.AcquisitionIndex][4] = (unsigned char)(lTempAngle);
        }
        else
        {
            ucLogData1[sDevice.AcquisitionIndex - LOG_DATA_BLOCK0_SIZE][0] = (unsigned char)(lTempTorque >> 16);
            ucLogData1[sDevice.AcquisitionIndex - LOG_DATA_BLOCK0_SIZE][1] = (unsigned char)(lTempTorque >> 8);
            ucLogData1[sDevice.AcquisitionIndex - LOG_DATA_BLOCK0_SIZE][2] = (unsigned char)lTempTorque;
            ucLogData1[sDevice.AcquisitionIndex - LOG_DATA_BLOCK0_SIZE][3] = (unsigned char)(lTempAngle >> 8);
            ucLogData1[sDevice.AcquisitionIndex - LOG_DATA_BLOCK0_SIZE][4] = (unsigned char)(lTempAngle);
        }
        sDevice.AcquisitionSize++;
        return 0;
    }
    else
    {
        //Não há mais espaço para armazenar o registro
        return 1;
    }
}
//
void PeakDetect(void)
{
    if(sDevice.AcquisitionDirection == CW)
    {
        if(lTempTorque > lADS1271PrevValue)
        {
            //Atingiu um novo pico
            //Salvar index
            sDevice.AcquisitionPeakIndex = sDevice.AcquisitionIndex;
            lADS1271PrevValue = lTempTorque;
        }
    }
    else
    {
       if(sDevice.AcquisitionDirection == CCW)
       {
           if(lTempTorque < lADS1271PrevValue)
           {
                //Atingiu um novo pico
                //Salvar index
                sDevice.AcquisitionPeakIndex = sDevice.AcquisitionIndex;
                lADS1271PrevValue = lTempTorque;                                       
           }
       }
    }    
}
//
void PeakDetectFreeTimer(void)
{
    if(sDevice.AcquisitionStatus == ACQ_THR || sDevice.AcquisitionStatus == ACQ_FINALTIMEOUT)
    {
        if(sDevice.AcquisitionDirection == CW)
        {
            if(lADS1271Value > sDevice.AcquisitionMaxTorque)
            {
                sDevice.AcquisitionMaxTorque = lADS1271Value;
                sDevice.AcquisitionPeakAngle = sEncoder.PulseAcc;
            }
        }
        else
        {
            if(sDevice.AcquisitionDirection == CCW)
            {
                if(lADS1271Value < sDevice.AcquisitionMaxTorque)
                {
                    sDevice.AcquisitionMaxTorque = lADS1271Value;
                    sDevice.AcquisitionPeakAngle = sEncoder.PulseAcc;
                }
            }
        }
    }
}
//
void Registry2Buffer(unsigned int RegistryAddress, unsigned char *ptrBuffer)
{
    if(RegistryAddress < LOG_DATA_BLOCK0_SIZE)
    {
        *ptrBuffer = ucLogData[RegistryAddress][0];
        ptrBuffer++;
        *ptrBuffer = ucLogData[RegistryAddress][1];
        ptrBuffer++;
        *ptrBuffer = ucLogData[RegistryAddress][2];
        ptrBuffer++;
        *ptrBuffer = ucLogData[RegistryAddress][3];
        ptrBuffer++;
        *ptrBuffer = ucLogData[RegistryAddress][4]; 
    }  
    else
    {
        *ptrBuffer = ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][0];
        ptrBuffer++;
        *ptrBuffer = ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][1];
        ptrBuffer++;
        *ptrBuffer = ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][2];
        ptrBuffer++;
        *ptrBuffer = ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][3];
        ptrBuffer++;
        *ptrBuffer = ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][4]; 

    } 
}
//
void TorqueLong2Registry(unsigned int RegistryAddress, long Registry)
{
    if(RegistryAddress < LOG_DATA_BLOCK0_SIZE)
    {
        ucLogData[RegistryAddress][0] = (unsigned char)(Registry >> 16);
        ucLogData[RegistryAddress][1] = (unsigned char)(Registry >> 8);
        ucLogData[RegistryAddress][2] = (unsigned char)Registry;
    }
    else
    {
        ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][0] = (unsigned char)(Registry >> 16);
        ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][1] = (unsigned char)(Registry >> 8);
        ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][2] = (unsigned char)Registry;
    }
}
//
long TorqueRegistry2Long(unsigned int RegistryAddress)
{
    if(RegistryAddress < LOG_DATA_BLOCK0_SIZE)
    {
        ucTemp = ucLogData[RegistryAddress][0];
        lTemp = ((long)ucTemp)<<16; 
        if(ucTemp >= 0x80)
            lTemp |= 0xFF000000;        
        ucTemp = (ucLogData[RegistryAddress][1]); 
        lTemp |= ((long)ucTemp) << 8;
        ucTemp = (ucLogData[RegistryAddress][2]); 
        lTemp |= (long)ucTemp;
    }
    else
    {
        ucTemp = ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][0];
        lTemp = ((long)ucTemp)<<16;
        if(ucTemp >= 0x80)
            lTemp |= 0xFF000000;
        ucTemp = (ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][1]); 
        lTemp |= ((long)ucTemp) << 8;
        ucTemp = (ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][2]); 
        lTemp |= (long)ucTemp;
    }
    return lTemp;
}
//
void Registry2BufferAscii(unsigned int RegistryAddress, unsigned char *ptrBuffer)
{
    if(RegistryAddress < LOG_DATA_BLOCK0_SIZE)
    {
        ucTempVar = ucLogData[RegistryAddress][0];
        ByteHex2Ascii(ucTempVar,ptrBuffer++);
        ptrBuffer++;
        ucTempVar = ucLogData[RegistryAddress][1];
        ByteHex2Ascii(ucTempVar,ptrBuffer++);
        ptrBuffer++;    
        ucTempVar = ucLogData[RegistryAddress][2];
        ByteHex2Ascii(ucTempVar,ptrBuffer++);
        ptrBuffer++;    
    }
    else
    {
        ucTempVar = ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][0];
        ByteHex2Ascii(ucTempVar,ptrBuffer++);
        ptrBuffer++;
        ucTempVar = ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][1];
        ByteHex2Ascii(ucTempVar,ptrBuffer++);
        ptrBuffer++;    
        ucTempVar = ucLogData1[RegistryAddress - LOG_DATA_BLOCK0_SIZE][2];
        ByteHex2Ascii(ucTempVar,ptrBuffer++);
        ptrBuffer++;  
    }
}

void IncCycleCounter(void)
{
    //Incrementa contador de ciclos
    sDevice.CycleCounter++;
    //Salva na NVRAM
    I2c1WriteLong(NVRAM_CYCLE_COUNTER, sDevice.CycleCounter);    
}
//
void IncMaxTorqueCounter(void)
{
    //Incrementa contador de sobretorque
    sDevice.OvertorqueCounter++;
    //Salva na NVRAM
    I2c1WriteLong(NVRAM_OVERTORQUE_COUNTER, sDevice.OvertorqueCounter);
}
//
void StringCapacity2Num(void)
{
    sDevice.NominalTorque = (sDevice.Capacity[0] - 0x30) * 100000;
    sDevice.NominalTorque += (sDevice.Capacity[1] - 0x30) * 10000;
    sDevice.NominalTorque += (sDevice.Capacity[2] - 0x30) * 1000;
    sDevice.NominalTorque += (sDevice.Capacity[3] - 0x30) * 100;
    sDevice.NominalTorque += (sDevice.Capacity[4] - 0x30) * 10;
    sDevice.NominalTorque += (sDevice.Capacity[5] - 0x30);
}
//
void CalcOverTorqueAdValue(void)
{
    StringCapacity2Num();
    l64Temp = sDevice.NominalTorque * 1200000000000;
    l64Temp = l64Temp / sDevice.TorqueSensitivity;
    sDevice.OverTorqueLimit = (long)l64Temp;
}
//
void ParametersInit(void)
{
    unsigned char index;
    unsigned long ulTemp;
    
    sDevice.BlueToothTrasparent = BT_NORMAL_MODE;
    //sDevice.BlueToothTrasparent = BT_TRANSPARENT_MODE;
    sBatVoltage.index = 0;
    sBatVoltage.Acummulated = 0;
    sBatVoltage.State = 0;
    sPwrButton.State = PWR_BUTTON_IDLE;
    if(PWR_CFG_PIN == 0)
        sPwrButton.Mode = ONOFF_ALWAYS_ON;
    else
        sPwrButton.Mode = ONOFF_PWR_BUTTON;
    sDevice.AutoPowerOff = 120;
    sDevice.PowerVoltage = 5133;
    sDevice.ErrorCode = 0xAC;
    sDevice.State = 0x00;
    sDevice.Temperature = 0x00;
    sDS18S20.Temperature = 0x00;
    sDevice.LastReset = RCON;
    
    sDevice.CommAct = 0;
    sDevice.CommActState =  STATUS_LED_NORMAL;
    
    sDevice.Alive = 0;
    //sDevice.CommInterface = RS232_INTERFACE;
    sDevice.AcquisitionType = TORQUEANGLE_ACQ;
    sDevice.AcquisitionReady = ACQ_NOT_READY;
    //sDevice.AcquisitionTimeStep = 1;
    sDevice.AnalogPowerState = ON;
    sDevice.BatState = BAT_NORMAL;
    sTorqueClick.Direction = STABLE;
    sDevice.FastScan = 0;
   
    ulTemp = I2c1ReadLong(NVRAM_AUTO_PWR_OFF_TIMER);
    if(ulTemp == 0)
    {
        //Auto PowerOff Timer desabilitado
        sDevice.AutoPowerOff = 0;
        TimerStop(AUTO_PWR_OFF_TIMER);
    }
    else
    {
        if(ulTemp >= MIN_AUTO_PWROFF_TIMER && ulTemp <= MAX_AUTO_PWROFF_TIMER)
        {
            //Auto PowerOff Timer habilitado
            sDevice.AutoPowerOff = ulTemp;
            //Dispara Timer
            TimerLoad(AUTO_PWR_OFF_TIMER, sDevice.AutoPowerOff * 1000, RUN);
        }
        else
        {
            //Não há um valor válido
            sDevice.AutoPowerOff = DEFAULT_AUTO_PWROFF_TIMER;
            TimerLoad(AUTO_PWR_OFF_TIMER, sDevice.AutoPowerOff * 1000, RUN);
        }
    }
    //
    sDevice.TorqueSensitivity = I2c1ReadLong(NVRAM_TORQUE_SENSITIVITY);
    sDevice.AngleSensivity = I2c1ReadLong(NVRAM_ANGLE_SENSITIVITY);
    sDevice.CalibTimeStamp = I2c1ReadLong(NVRAM_CALIB_TIMESTAMP);
    sDevice.CalibData = I2c1ReadLong(NVRAM_CALIB_DATA);    
    sDevice.CycleCounter = I2c1ReadLong(NVRAM_CYCLE_COUNTER);
    sDevice.OvertorqueCounter = I2c1ReadLong(NVRAM_OVERTORQUE_COUNTER);
    sDevice.MaxAppliedTorque = I2c1ReadLong(NVRAM_MAX_APPLIED_TORQUE);
    lTorqueOffset = I2c1ReadLong(NVRAM_TORQUE_OFFSET);
    //
    sDevice.PowerSource = EXTERNAL_DC;
    sDevice.PowerState = 5;
    sDevice.ErrorCode = NO_ERROR;
    sDevice.TransducerMaxPositive = TORQUE_OPEN_TRANSDUCER;
    sDevice.TransducerMaxNegative = TORQUE_OPEN_TRANSDUCER * (-1);
    //
    sDevice.AcquisitionStatus = ACQ_IDLE;
    sDevice.AcquisitionIndex = 0;
    sDevice.AcquisitionTHFIndex = 0;
    sDevice.AcquisitionTimeStep = 1;
    sDevice.AcquisitionFinalTimeout = 100;
    sDevice.AcquisitionDirection = CW;
    sDevice.AcquisitionExpectedDirection = CW;
    sDevice.AcquisitionSize = 0;
    sDevice.FilterFreq = FILTER_FREQ_DEFAULT;
    //
    lADS1271ValueMax = 0;
    lADS1271ValueMin = 0;
    //
    sDevice.InitialThreshold = 0x00051FE9;      //2Nm
    sDevice.FinalThreshold = 0x00051FE9 / 2;    //1Nm   
    //
    //Carrega os valores da ROM
    //
    //Serial Number
    for(index = 0; index < SERIAL_SIZE; index++)
        sDevice.SerialNumber[index] = I2c1ReadByte(NVRAM_SERIAL_NUMBER+index);

    //Model
    for(index = 0; index < MODEL_SIZE; index++)
        sDevice.Model[index] = I2c1ReadByte(NVRAM_MODEL+index);
    //
    //Hardware Version
    for(index = 0; index < VERSION_SIZE; index++)
        sDevice.HwVersion[index] = romHwVersion[index];    
    //
    //Firmware Version
    for(index = 0; index < VERSION_SIZE; index++)
        sDevice.FwVersion[index] = romFwVersion[index]; 
    //
    //Capacidade
    for(index = 0; index < CAPACITY_SIZE; index++)
        sDevice.Capacity[index] = I2c1ReadByte(NVRAM_CAPACITY+index);

    //TYPE
    for(index = 0; index < TYPE_SIZE; index++)
        sDevice.Type[index] = romType[index];  
    //
    CalcOverTorqueAdValue();
    //
    ulTemp = I2c1ReadWord(NVRAM_CLICK_FALL);
    if(ulTemp >= MIN_CLICK_FALL && ulTemp <= MAX_CLICK_FALL)
        sDevice.ClickFall = ulTemp;
    else
        sDevice.ClickFall = DEFAULT_CLICK_FALL;
    //
    //
    ulTemp = I2c1ReadWord(NVRAM_CLICK_RISE);
    if(ulTemp >= MIN_CLICK_RISE && ulTemp <= MAX_CLICK_RISE)
        sDevice.ClickRise = ulTemp;
    else
        sDevice.ClickRise = DEFAULT_CLICK_RISE;
    //
    //
    ulTemp = I2c1ReadWord(NVRAM_CLICK_WITDH);
    if(ulTemp >= MIN_CLICK_WIDTH && ulTemp <= MAX_CLICK_WIDTH)
        sDevice.ClickWidth = ulTemp;
    else
        sDevice.ClickWidth = DEFAULT_CLICK_WIDTH;    
    sDevice.AcquisitionResetTimeout = ACQ_RESET_TIMEOUT_DEFAULT;
    sDevice.AcquisitionDelayTimeout = ACQ_DELAY_DEFAULT;
}
//
void GetId(void)
{
    if(DS18S20GetROM() != 0x00)
    {
        //Erro na leitura do ID
        sDS18S20.SerialNumber[5] = 0xFF;
        sDS18S20.SerialNumber[4] = 0xFF;
        sDS18S20.SerialNumber[3] = 0xFF;
        sDS18S20.SerialNumber[2] = 0xFF;
        sDS18S20.SerialNumber[1] = 0xFF;
        sDS18S20.SerialNumber[0] = 0xFF;
    }
    //
    ByteHex2Ascii(sDS18S20.SerialNumber[5],ucIdAscii);
    ByteHex2Ascii(sDS18S20.SerialNumber[4],ucIdAscii+2);
    ByteHex2Ascii(sDS18S20.SerialNumber[3],ucIdAscii+4);
    ByteHex2Ascii(sDS18S20.SerialNumber[2],ucIdAscii+6);
    ByteHex2Ascii(sDS18S20.SerialNumber[1],ucIdAscii+8);
    ByteHex2Ascii(sDS18S20.SerialNumber[0],ucIdAscii+10);   
    //
    ByteHex2Ascii(sDevice.ExpAddress,ucIdAsciiAddress);
    ByteHex2Ascii(sDS18S20.SerialNumber[4],ucIdAsciiAddress+2);
    ByteHex2Ascii(sDS18S20.SerialNumber[3],ucIdAsciiAddress+4);
    ByteHex2Ascii(sDS18S20.SerialNumber[2],ucIdAsciiAddress+6);
    ByteHex2Ascii(sDS18S20.SerialNumber[1],ucIdAsciiAddress+8);
    ByteHex2Ascii(sDS18S20.SerialNumber[0],ucIdAsciiAddress+10); 
}
//
void GetAddress(void)
{
    #if(SMARTCLICK_ENABLE == 1)
        sDevice.ExpAddress = 0;
        ucTempVar = 0;//(ADDR1_EXP_PIN)& 0x01;
        ucTempVar = ucTempVar << 1;
        sDevice.ExpAddress |= ucTempVar;
        ucTempVar = (ADDR0_EXP_PIN)& 0x01;
        sDevice.ExpAddress |= ucTempVar;
        sDevice.ExpAddress +=1;
    #else
        sDevice.ExpAddress = 0;
        ucTempVar = (ADDR1_EXP_PIN)& 0x01;
        ucTempVar = ucTempVar << 1;
        sDevice.ExpAddress |= ucTempVar;
        ucTempVar = (ADDR0_EXP_PIN)& 0x01;
        sDevice.ExpAddress |= ucTempVar;
        sDevice.ExpAddress +=1;
    #endif
}
//
void ComInit(void)
{
    COM2Init(BAUD_115200);              //USB
    //    
    #if(PRODUCT_TYPE == 0)
        COM1Init(BAUD_115200);   //ESP8266
        BLUETOOTH_PWR_ENABLE_PIN = OFF;
    #elif(PRODUCT_TYPE == 1)
        #if(HC06_CONN == 2)
            BLUETOOTH_PWR_ENABLE_PIN = ON;
            COM1Init(BAUD_115200);   //ESP8266
        #else
            BLUETOOTH_PWR_ENABLE_PIN = ON;
            COM1Init(BAUD_38400);   //Blue Tooth
        #endif
    #elif(PRODUCT_TYPE ==2)
        BLUETOOTH_PWR_ENABLE_PIN = ON;
        COM1Init(BAUD_38400);   //Blue Tooth
    #endif 
    //
    #if HC06_CONN == 2
        //ESP8266Init();
        //Inicia pulso de reset para o ESP8266
        #if(SMARTCLICK_ENABLE == 1)
            ESP8266_RESET_PIN = ON;
            TimerDelay(DELAYTIMER, 100);
            ESP8266_MODE_PIN = OFF;
            TimerDelay(DELAYTIMER, 100);
            ESP8266_RESET_PIN = OFF;
            TimerDelay(DELAYTIMER, 300);                   
        #endif
        BLUETOOTH_PWR_ENABLE_PIN = ESP_RESET_ON;
        TimerLoad(DELAYTIMER,100,RUN);
        while(TimerStatus(DELAYTIMER)==RUN)
        {
            PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
            ClrWdt();
        }  
        BLUETOOTH_PWR_ENABLE_PIN = ESP_RESET_OFF;
    #endif
}
//
void HC06Config(void)
{
#if HC06_CONFIG == 1
    TimerLoad(DELAYTIMER,1000,RUN);
    while(TimerStatus(DELAYTIMER)==RUN)
    {
        PWR_ENABLE_PIN = ~PWR_ENABLE_PIN; 
        ClrWdt();
    }
    BlueToothCmd("AT+BAUD8",8);
    TimerLoad(DELAYTIMER,500,RUN);
    while(TimerStatus(DELAYTIMER)==RUN)
    {
        PWR_ENABLE_PIN = ~PWR_ENABLE_PIN; 
        ClrWdt();
    }   
    Nop();
    Nop();
    Nop();
    Nop();    
    BlueToothCmd("AT",2);
    TimerLoad(DELAYTIMER,500,RUN);
    while(TimerStatus(DELAYTIMER)==RUN)
    {
        PWR_ENABLE_PIN = ~PWR_ENABLE_PIN; 
        ClrWdt();
    }  
    Nop();
    Nop();
    Nop();
    Nop();
    BlueToothCmd("AT+NAMEPhoenix 10Nm",19);
    TimerLoad(DELAYTIMER,500,RUN);
    while(TimerStatus(DELAYTIMER)==RUN)
    {
        PWR_ENABLE_PIN = ~PWR_ENABLE_PIN; 
        ClrWdt();
    }   
    Nop();
    Nop();
    Nop();
    Nop();
    BlueToothCmd("AT+PIN0000",10);
    TimerLoad(DELAYTIMER,500,RUN);
    while(TimerStatus(DELAYTIMER)==RUN)
    {
        PWR_ENABLE_PIN = ~PWR_ENABLE_PIN; 
        ClrWdt();
    }
    Nop();
    Nop();
    Nop();
    Nop();

#endif
}
//
void LedRefresh(void)
{
    if(TimerStatus(LED_TIMER)==OVERFLOW)
    {
            if(sDevice.CommAct > 0)
            {
                LED_YELLOW_PIN = ~LED_YELLOW_PIN;
                sDevice.CommAct--;
                LED_GREEN_PIN = _OFF;
                TimerLoad(LED_TIMER,COMM_LED_RATE,RUN);
            }
            else
            {
                LED_GREEN_PIN = ~LED_GREEN_PIN;
                TimerLoad(LED_TIMER,300,RUN);
                LED_YELLOW_PIN = _OFF;
                sDevice.CommActState = STATUS_LED_NORMAL;
            }
        
    }
}
//
void CheckComMode(void)
{
    //Verifica sair do modo transparente
    if(ucTransparentEscape == 1 && sDevice.BlueToothTrasparent == BT_TRANSPARENT_MODE)
    {
        //Sair do modo transparente
        asm ("RESET");
    }
    if(sDevice.BlueToothTrasparent == 1)
    {
        if(sCOM2RxBuffer.Size > 0)
        {
            //Verifica se houve altera��o na quantidade de bytes recebidos
            if(sCOM2RxBuffer.Size != sCOM2RxBuffer.MessageSize)
            {
                //A quantidade � diferente
                sCOM2RxBuffer.MessageSize = sCOM2RxBuffer.Size;
                //Para Timer
                TimerStop(UART_TIMER);
            }
            else
            {
                //As quantidade de bytes são iguais
                //Disparar timer
                if(TimerStatus(UART_TIMER) == STOP)
                    TimerLoad(UART_TIMER, 25, RUN);                    
            }
            if(TimerStatus(UART_TIMER) == OVERFLOW)
            {
                //Ocorreu Overflow
                //Não foram recebidos mais bytes
                //Notificar que há job pendente na serial                    
                TimerStop(UART_TIMER);

                //SerialRxBufferReset();                    
                for(uiTempVar = 0;uiTempVar < sCOM2RxBuffer.MessageSize; uiTempVar++)
                {
                    sCOM1TxBuffer.Data[uiTempVar] = sCOM2RxBuffer.Data[uiTempVar];   //Move primeiro byte do comando
                }
                sCOM1TxBuffer.Pointer = 0;
                sCOM1TxBuffer.Size = sCOM2RxBuffer.MessageSize;
                sCOM2RxBuffer.Size = 0;
                sCOM2RxBuffer.Pointer = 0;
                sCOM1TxBuffer.Status = SLOWCOMM_TX_EMPTY;
                COM1TxStart();
            }
        }
    }
}

