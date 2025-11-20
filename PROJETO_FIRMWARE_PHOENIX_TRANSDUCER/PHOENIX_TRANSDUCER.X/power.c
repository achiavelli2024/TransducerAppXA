#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "timer.h"
#include "ds18s20.h"
#include "ad.h"
//
extern DeviceStruct sDevice;
extern BatVoltageStruct sBatVoltage;
extern PwrButtonStruct sPwrButton;

void AnalogPowerControl(unsigned char state)
{
    if(state == 1)
    {
        //Liga a fonte de alimenta��o anal�gica
        AVCC_ENABLE_PIN = ON;
    }
    else
    {
        //Desliga a fonte de alimenta��o anal�gica
        AVCC_ENABLE_PIN = OFF;
    }
}
//
void EncoderPowerControl(unsigned char state)
{
    if(state == 1)
    {
        //Liga a fonte de alimenta��o do encoder
        ENCVCC_ENABLE_PIN = ON;
    }
    else
    {
        //Desliga a fonte de alimenta��o do encoder
        ENCVCC_ENABLE_PIN = OFF;
    }
}
//
void PowerStatus(void)
{
    //Atualiza os estados do Power
    //
    unsigned char i;
    unsigned char temp;
    if(POWERGOOD_PIN)
        sDevice.PowerSource = PWR_SOURCE_BAT;
    else
        sDevice.PowerSource = PWR_SOURCE_EXT;
    //
    //Obtém estado do carregador
    i = BAT_STAT1_PIN << 1;
    i |= BAT_STAT2_PIN;
    
    if(sDevice.PowerSource == PWR_SOURCE_EXT)
    {
        sDevice.PowerState = i;
        TimerStop(CRITICAL_BAT_TIMER);
    }
    else
        sDevice.PowerState = sDevice.BatState;
    //
    switch(sBatVoltage.State)
    {
        case BAT_SENSE_IDLE:
        {
            temp = TimerStatus(BAT_SENSE_TIMER);
            if(temp == OVERFLOW)
            {
                //Habilita FET 
                BAT_SENSE_ENABLE_PIN = ON;
                TimerLoad(BAT_SENSE_TIMER,BAT_SENSE_ENABLE_TIMEOUT,RUN);
                sBatVoltage.State = BAT_SENSE_ENABLE;
            }
            else
            {
                if(temp == STOP)
                {
                    TimerLoad(BAT_SENSE_TIMER,BAT_SENSE_IDLE_TIMEOUT,RUN);
                }
            }
            break;
        }
        //
        case BAT_SENSE_ENABLE:
        {
            if(TimerStatus(BAT_SENSE_TIMER) == OVERFLOW)
            {
                //A condu��o do FET est� estabilizada
                //Iniciar a amostragem
                sBatVoltage.index = 0;
                sBatVoltage.Acummulated = 0;
                sBatVoltage.State = BAT_SENSE_SAMPLE;
                #if(TEMPERATURE_SENSOR_ENABLE == 1)
                    //Obter temperatura
                    if(DS18S20GetRAM() == 0)    //Obt�m temperatura
                    {
                        //Houve sucesso ao obter a temperatura
                        sDevice.Temperature = sDS18S20.Temperature;       //Atualiza a temperatura
                    }
                    else
                    {
                        //Erro ao obter a temperatura
                        sDevice.Temperature = 0; 
                    }
                    DS18S20StartConversion();                             //Dispara nova convers�o
                #endif
            }
            break;
        }
        //
        case BAT_SENSE_SAMPLE:
        {
            if(sBatVoltage.index < BAT_AD_SAMPLES)
            {
                sBatVoltage.Acummulated += (unsigned long)GetAnalog(BAT_AD_CHANNEL);
                sBatVoltage.index++;
            }
            else
            {
                sBatVoltage.Acummulated = sBatVoltage.Acummulated / BAT_AD_SAMPLES;
                sBatVoltage.Acummulated = (sBatVoltage.Acummulated * BAT_AD_MULTIPLIER) / BAT_AD_DIVIDER;
                sDevice.PowerVoltage = (unsigned int)sBatVoltage.Acummulated; 
                sBatVoltage.index = 0;
                sBatVoltage.Acummulated = 0;
                sBatVoltage.State = BAT_SENSE_IDLE;
                //Desabilita FET
                BAT_SENSE_ENABLE_PIN = OFF;
                TimerStop(BAT_SENSE_TIMER);
                //Testa o n�vel de bateria
                //
                if(sDevice.PowerVoltage > BAT_LOW_LEVEL)
                {
                    sDevice.BatState = BAT_NORMAL;
                    TimerStop(CRITICAL_BAT_TIMER);
                }
                else
                {
                    if(sDevice.PowerVoltage <= BAT_LOW_LEVEL && sDevice.PowerVoltage > BAT_CRITICAL_LEVEL )
                    {
                        sDevice.BatState = BAT_LOW;
                        TimerStop(CRITICAL_BAT_TIMER);
                    }
                    else
                    {
                        if(sDevice.PowerVoltage <= BAT_CRITICAL_LEVEL)
                        {
                            sDevice.BatState = BAT_CRITICAL;
                            if(TimerStatus(CRITICAL_BAT_TIMER) == STOP)
                            {
                                TimerLoad(CRITICAL_BAT_TIMER,BAT_CRITICAL_TURNOFF_TIMEOUT,RUN);
                            }
                            else
                            {
                                if(TimerStatus(CRITICAL_BAT_TIMER) == OVERFLOW)
                                {
                                    //Ocorreu overflow
                                    //Desligar para preservar bateria
                                     while(1)
                                    {
                                        ClrWdt();
                                    }
                                }
                            }
                        }
                    }
                }
            }            
            break;
        }
    }
}
//
void PowerSupplyInit(void)
{
    AnalogPowerControl(ON);
#if(PRODUCT_TYPE == 0)
    EncoderPowerControl(ON);
    sDevice.EncoderPowerState = ON;
#elif(PRODUCT_TYPE == 1)
    EncoderPowerControl(ON);
    sDevice.EncoderPowerState = ON;
#endif    
}
//
unsigned char PwrButtonCheck(unsigned char ucData)
{
    unsigned char temp = 0;
    if(sPwrButton.Mode == ONOFF_ALWAYS_ON)
        return 1;
    else
    {
        switch (sPwrButton.State)
        {
            case PWR_BUTTON_IDLE:
            {
                temp = 1;
                if(ON_SW_PIN)
                {
                    temp = 2;
                    if(ucData == OFF)
                        TimerLoad(PWR_BUTTON_TIMER,PWR_BUTTON_OFF_PRESS_TIME,RUN);
                    else
                        TimerLoad(PWR_BUTTON_TIMER,PWR_BUTTON_ON_PRESS_TIME,RUN);
                    sPwrButton.State = PWR_BUTTON_PRESSED;
                }
                break;
            }
            //
            case PWR_BUTTON_PRESSED:
            {
                temp = 2;
                if(!ON_SW_PIN)
                {
                    //O bot�o foi solto antes do tempo
                    sPwrButton.State = PWR_BUTTON_IDLE;
                    TimerStop(DELAYTIMER);
                }
                else
                {
                    if(TimerStatus(PWR_BUTTON_TIMER)==OVERFLOW)
                    {
                        //Atingiu o tempo
                        //Desligar dispositivo                        
                        sPwrButton.State = PWR_BUTTON_RELEASE;
                        temp = 3;
                    }
                }
                break;
            }
            //
            case PWR_BUTTON_RELEASE:
            {
//                if(!ON_SW_PIN)
//                    temp = 2;
//                else
//                    temp = 3;
                temp = 3;
                break;
            }
        }
        return temp;
    }
}
//
void PowerButtonScan(void)
{
    if(PwrButtonCheck(OFF) < 3)
        PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
    else
    {
        //Desligar dispositivo
        LED_YELLOW_PIN = _OFF;
        Nop();
        Nop();
        LED_GREEN_PIN = _OFF;
        Nop();
        Nop();            
        BLUETOOTH_PWR_ENABLE_PIN = OFF;
        TimerLoad(DELAYTIMER,500,RUN);
        while(TimerStatus(DELAYTIMER)==RUN)
        {
            PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
            ClrWdt();
        }
        while(1)
        {
            ClrWdt();
        }
    }
    //Verifica o Auto Power OFF Timer
    if(TimerStatus(AUTO_PWR_OFF_TIMER) == OVERFLOW)
    {
        //O timer contou até o final
        //Desligar dispositivo por inatividade
        if(sDevice.BlueToothTrasparent == BT_NORMAL_MODE)
        {
            while(1)
            {
                ClrWdt();
            }
        }
    }
} 

