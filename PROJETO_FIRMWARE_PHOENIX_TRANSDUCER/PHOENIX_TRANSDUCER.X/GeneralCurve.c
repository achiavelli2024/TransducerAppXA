#include <stdio.h>
#include <stdlib.h>
//#include <libq.h>
#include "hardware.h"
#include "timer.h"
#include "ads1271.h"
#include "encoder.h"
#include "peakfinder.h"

extern long lTempAngle, lTempTorque, lTemp, lTorqueOffset;
extern DeviceStruct sDevice, sTempDevice;
extern PeakFinderStruct sPeakFinder;

void PeakDetect(void);
void IncCycleCounter(void);
void IncMaxTorqueCounter(void);
unsigned char StoreData(void);

void GenericCurve(void)
{
    if(sDevice.AnalogPowerState == ON)
    {
        lTempTorque = lADS1271Value - lTorqueOffset;
        lTempAngle = sEncoder.PulseAcc;
        //A Alimentação analógica está ligada
        if(sDevice.AcquisitionReady == ACQ_READY)
        {
            //Está no modo DataLogger
            switch(sDevice.AcquisitionStatus)
            {
                case ACQ_IDLE:
                {
                    //Ainda não atingiu o valor de Threshold
                    //Verificar se o Threshold foi alcançado
                    if(lTempTorque >= sDevice.InitialThreshold)
                    {
                        if(sDevice.AcquisitionExpectedDirection == CW || sDevice.AcquisitionExpectedDirection == CWCCW)
                        {
                            //***
                            //Threshold alcançado para o sentido direito
                            //Zera �ngulo
                            sEncoder.PulseAcc = 0;
                            lTempAngle = 0;
                            //Iniciar armazenamento da curva
                            sDevice.AcquisitionIndex = 0;
                            sDevice.AcquisitionPeakIndex = 0;
                            sDevice.AcquisitionTHFIndex = 0;
                            sDevice.AcquisitionSize = 0;
                            sDevice.AcquisitionMaxTorque = 0;
                            sDevice.AcquisitionPeakAngle = 0;
                            sDevice.MaxAppliedTorqueChecked = 0;
                            lADS1271PrevValue = lTempTorque;
                            //IncCycleCounter();
                            sDevice.FastScan = 0;
                            
                            if(sDevice.AcquisitionDelayTimeout > 0 && sDevice.AcquisitionDelayFlag == 0)
                            {
                                //O delay inicial está habilitado
                                TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionDelayTimeout,RUN);
                                sDevice.AcquisitionStatus = ACQ_DELAY_TIME;
                            }
                            else
                            {
                                sDevice.AcquisitionStatus = ACQ_THR;
                                sDevice.AcquisitionDirection = CW;
                            }
                        }
                    }
                    else
                    {
                        //Verificar se atingiu o Threshold para o sentido esquerdo
                        //Obtém módulo do torque
                        lTemp = 0 - lTempTorque;
                        if(lTemp >= sDevice.InitialThreshold)
                        {
                            if(sDevice.AcquisitionExpectedDirection == CCW || sDevice.AcquisitionExpectedDirection == CWCCW)
                            {
                                //****
                                //Threshold alcançado para o sentido esquerdo
                                //Zera ângulo
                                sEncoder.PulseAcc = 0;
                                lTempAngle = 0;
                                //Iniciar armazenamento da curva
                                sDevice.AcquisitionIndex = 0;
                                sDevice.AcquisitionPeakIndex = 0;
                                sDevice.AcquisitionSize = 0;
                                sDevice.AcquisitionTHFIndex = 0;
                                sDevice.AcquisitionMaxTorque = 0;
                                sDevice.AcquisitionPeakAngle = 0;
                                sDevice.MaxAppliedTorqueChecked = 0;
                                lADS1271PrevValue = lTempTorque;
                                //IncCycleCounter();
                                sDevice.FastScan = 0;
                                if(sDevice.AcquisitionDelayTimeout > 0 && sDevice.AcquisitionDelayFlag == 0)
                                {
                                    //O delay inicial está habilitado
                                    TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionDelayTimeout,RUN);
                                    sDevice.AcquisitionStatus = ACQ_DELAY_TIME;
                                } 
                                else
                                {
                                    //Mudar para próximo estado
                                    sDevice.AcquisitionStatus = ACQ_THR;
                                    sDevice.AcquisitionDirection = CCW;
                                }
                            }
                        }
                    }
                    break;
                }
                //
                case ACQ_THR:
                {
                    //Armazenar a curva até encontrar o Threshold final
                    PeakDetect();
                    //Verifica se alcançou o Threshold final
                    if(sDevice.AcquisitionDirection == CW)
                    {
                        //Detectar o Threshold final positivo
                        if(lTempTorque <= sDevice.FinalThreshold)
                        {
                            //Detectado Threshold final
                            //Disparar timer e mudar de estado
                            //FINAL_THRESHOLD_TIMER
                            
                            if(sDevice.AcquisitionResetTimeout > 0)
                            {
                                TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionResetTimeout,RUN);
                                sDevice.AcquisitionStatus = ACQ_RESET_TIME;  
                            }
                            else
                            {
                                TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionFinalTimeout,RUN);
                                sDevice.AcquisitionStatus = ACQ_FINALTIMEOUT;
                                sDevice.AcquisitionTHFIndex = sDevice.AcquisitionIndex;
                            }
                        }
                    }
                    else
                    {
                        if(sDevice.AcquisitionDirection == CCW)
                        {
                            //Detectar o Threshold final negativo
                            lTemp = 0 - lTempTorque;
                            if(lTemp <= sDevice.FinalThreshold)
                            {
                                //Detectado Threshold final
                                //Disparar timer e mudar de estado
                                TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionFinalTimeout,RUN);
                                sDevice.AcquisitionStatus = ACQ_FINALTIMEOUT;
                                sDevice.AcquisitionTHFIndex = sDevice.AcquisitionIndex;
                            }
                        }
                    }
                    break;
                }
                //
                case ACQ_FINALTIMEOUT:
                {
                    //Verificar se atingiu o limite de tempo para encerrar
                    if(TimerStatus(FINAL_THRESHOLD_TIMER)==OVERFLOW)
                    {
                        /*
                        //Inicia a análise dos picos
                        Nop();
                        Nop();
                        Nop();                          
                        sPeakFinder.DerivativeTheshold = 36446;
                        sPeakFinder.DataLenght = sDevice.AcquisitionSize;
                        GetMinMax(sDevice.AcquisitionSize);
                        sPeakFinder.fMaxValue = AdCount2Torque(sPeakFinder.MaxValue);//(TorqueRegistry2Long(sPeakFinder.MaxValueIndex));
                        sPeakFinder.fMinValue = AdCount2Torque(sPeakFinder.MinValue);//(TorqueRegistry2Long(sPeakFinder.MinValueIndex));
                        Nop();
                        Nop();
                        Nop();                          
                        sPeakFinder.ValueThreshold = sDevice.InitialThreshold/2;//TorqueRegistry2Long(sPeakFinder.MaxValueIndex);
                        PeakFinder(sDevice.AcquisitionSize);
                        Nop();
                        Nop();
                        Nop();                          
                        GetMinMaxPeak(sPeakFinder.PeakIndexList, sPeakFinder.PeakCounter);
                        sPeakFinder.fMaxPeakValue = AdCount2Torque(sPeakFinder.MaxPeakValue);//(TorqueRegistry2Long(sPeakFinder.PeakIndexList[sPeakFinder.MaxPeakValueIndex]));
                        sPeakFinder.fMinPeakValue = AdCount2Torque(sPeakFinder.MinPeakValue);//(TorqueRegistry2Long(sPeakFinder.PeakIndexList[sPeakFinder.MinPeakValueIndex]));
                        Nop();
                        Nop();
                        Nop();                          
                        sPeakFinder.fPeakValue = AdCount2Torque(sPeakFinder.MaxPeakValue);//(sPeakFinder.PeakIndexList[sPeakFinder.MaxPeakValueIndex]);
                        Nop();
                        Nop();
                        Nop();                          
                        long long tempvar = sPeakFinder.MaxPeakValue;//TorqueRegistry2Long(sPeakFinder.PeakIndexList[sPeakFinder.MaxPeakValueIndex]);
                        tempvar = tempvar * (sPeakFinder.ValidPeakPercentage * 10);
                        tempvar = tempvar / 1000;
                        sPeakFinder.PeakThreshold = tempvar;
                        sPeakFinder.fPeakThreshold = AdCount2Torque(sPeakFinder.PeakThreshold);
                        PeakAverage(sPeakFinder.PeakThreshold);
                        Nop();                        
                        sPeakFinder.fPeakAverage = AdCount2Torque(sPeakFinder.PeakAverage);
                        //sDevice.AcquisitionMaxTorque = (long)sPeakFinder.PeakAverage;
                        Nop();
                        Nop();
                        Nop();  
                        
                        //Fim
                        */
                        //Atingiu o tempo limite
                        //Terminar aquisição
                        sDevice.AcquisitionStatus = ACQ_FINISHED;
                    }
                    else
                    {
                        //verfica se ultrapassou o Threshold final
                        //Caso sim, zerar timer
                        if(sDevice.AcquisitionDirection == CW)
                        {
                            if(lTempTorque >= sDevice.FinalThreshold)
                            {
                                //ultrapassou o Threshold final
                                //Parar timer
                                TimerStop(FINAL_THRESHOLD_TIMER);
                                //Mudar para o estado ACQ_THR
                                sDevice.AcquisitionStatus = ACQ_THR;
                            }
                        }
                        else
                        {
                            if(sDevice.AcquisitionDirection == CCW)
                            {
                                lTemp = 0 - lTempTorque;
                                if(lTemp >= sDevice.FinalThreshold)
                                {
                                    //ultrapassou o Threshold final
                                    //Parar timer
                                    TimerStop(FINAL_THRESHOLD_TIMER);
                                    //Mudar para o estado ACQ_THR
                                    sDevice.AcquisitionStatus = ACQ_THR;
                                }
                            }
                        }
                        PeakDetect();
                    }
                    break;
                }
                //
                case ACQ_FINISHED:
                {
                    //Aquisição finalizada
                    //
                    //Incrementa contador de ciclos

                    sDevice.AcquisitionDelayFlag = 0;
                    //Testa para verificar sobretorque
                    if(sDevice.MaxAppliedTorqueChecked == 0)
                    {
                        IncCycleCounter();
                        //testa se houve sobretorque
                        sDevice.MaxAppliedTorqueChecked = 1;
                        if(sDevice.AcquisitionDirection == CW)
                        {
                            if(sDevice.MaxAppliedTorque > sDevice.OverTorqueLimit)
                            {
                                //Ocorreu um sobretorque no sentido horário
                                //Incrementar contador de sobretorque
                                IncMaxTorqueCounter();
                            }
                        }
                        else
                        {
                            if(sDevice.AcquisitionDirection == CCW)
                            {
                                lTemp = 0 - sDevice.MaxAppliedTorque;
                                if(lTemp > sDevice.OverTorqueLimit)
                                {
                                    //Ocorreu um sobretorque no sentido anti-horário
                                    //Incrementar contador de sobretorque
                                    IncMaxTorqueCounter();
                                }
                            }
                        }
                    }
                    break;
                }
                //
                case ACQ_DELAY_TIME:
                {
                    if(TimerStatus(FINAL_THRESHOLD_TIMER)==OVERFLOW)
                    {
                        sDevice.AcquisitionDelayFlag = 1;
                        //sDevice.AcquisitionStatus = ACQ_IDLE;
                        sDevice.AcquisitionStatus = ACQ_THR;
                    }
                    break;
                }
                //
                case ACQ_RESET_TIME:
                {
                    if(TimerStatus(FINAL_THRESHOLD_TIMER)==OVERFLOW)
                    {
                        TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionFinalTimeout,RUN);
                        sDevice.AcquisitionStatus = ACQ_FINALTIMEOUT;
                        sDevice.AcquisitionTHFIndex = sDevice.AcquisitionIndex;
                    }
                    break;
                }
                //
                default:
                {
                    break;
                }
                //
            }
                    //
        }
    }
}

/*
void GenericCurve(void)
{
    //long lTemp;
    TimerLoad(ACQ_TIMER,sDevice.AcquisitionTimeStep,RUN);
    if(sDevice.AnalogPowerState == ON)
    {
        lTempTorque = lADS1271Value;
        lTempAngle = sEncoder.PulseAcc;
        //A Alimenta��o anal�gica est� ligada
        if(sDevice.AcquisitionReady == ACQ_READY)
        {
            //Est� no modo DataLogger
            switch(sDevice.AcquisitionStatus)
            {
                case ACQ_IDLE:
                {
                    //Ainda n�o atingiu o valor de Threshold
                    //Verificar se o Threshold foi alcan�ado
                    if(lTempTorque >= sDevice.InitialThreshold)
                    {
                        if(sDevice.AcquisitionExpectedDirection == CW || sDevice.AcquisitionExpectedDirection == CWCCW)
                        {
                            //
                            //Threshold alcan�ado para o sentido direito
                            //Zera �ngulo
                            sEncoder.PulseAcc = 0;
                            lTempAngle = 0;
                            //Iniciar armazenamento da curva
                            sDevice.AcquisitionIndex = 0;
                            sDevice.AcquisitionPeakIndex = 0;
                            sDevice.AcquisitionTHFIndex = 0;
                            sDevice.AcquisitionSize = 0;
                            sDevice.AcquisitionMaxTorque = 0;
                            sDevice.AcquisitionPeakAngle = 0;
                            sDevice.MaxAppliedTorqueChecked = 0;
                            lADS1271PrevValue = lTempTorque;
                            //IncCycleCounter();
                            sDevice.FastScan = 0;
                            
                            if(sDevice.AcquisitionDelayTimeout > 0 && sDevice.AcquisitionDelayFlag == 0)
                            {
                                //O delay inicial est� habilitado
                                TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionDelayTimeout,RUN);
                                sDevice.AcquisitionStatus = ACQ_DELAY_TIME;
                            }
                            else
                            {
                                if(StoreData() == 0)
                                {   
                                    //Aponta para o pr�ximo registro
                                    sDevice.AcquisitionIndex++;
                                    //Mudar para pr�ximo estado
                                    sDevice.AcquisitionStatus = ACQ_THR;
                                    sDevice.AcquisitionDirection = CW;
                                    //sDevice.AcquisitionSize++;
                                }
                                else
                                {
                                    //Ocorreu erro ao tentar armazenar o registro
                                    sDevice.AcquisitionStatus = ACQ_FINISHED;
                                }
                                //
                            }
                        }
                    }
                    else
                    {
                        //Verificar se atingiu o Threshold para o sentido esquerdo
                        //Obt�m m�dulo do torque
                        lTemp = 0 - lTempTorque;
                        if(lTemp >= sDevice.InitialThreshold)
                        {
                            if(sDevice.AcquisitionExpectedDirection == CCW || sDevice.AcquisitionExpectedDirection == CWCCW)
                            {
                                //
                                //Threshold alcan�ado para o sentido esquerdo
                                //Zera �ngulo
                                sEncoder.PulseAcc = 0;
                                lTempAngle = 0;
                                //Iniciar armazenamento da curva
                                sDevice.AcquisitionIndex = 0;
                                sDevice.AcquisitionPeakIndex = 0;
                                sDevice.AcquisitionSize = 0;
                                sDevice.AcquisitionTHFIndex = 0;
                                sDevice.AcquisitionMaxTorque = 0;
                                sDevice.AcquisitionPeakAngle = 0;
                                sDevice.MaxAppliedTorqueChecked = 0;
                                lADS1271PrevValue = lTempTorque;
                                //IncCycleCounter();
                                sDevice.FastScan = 0;
                                if(sDevice.AcquisitionDelayTimeout > 0 && sDevice.AcquisitionDelayFlag == 0)
                                {
                                    //O delay inicial est� habilitado
                                    TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionDelayTimeout,RUN);
                                    sDevice.AcquisitionStatus = ACQ_DELAY_TIME;
                                } 
                                else
                                {
                                    if(StoreData() == 0)
                                    {   
                                        //Aponta para o pr�ximo registro
                                        sDevice.AcquisitionIndex++;
                                        //Mudar para pr�ximo estado
                                        sDevice.AcquisitionStatus = ACQ_THR;
                                        sDevice.AcquisitionDirection = CCW;
                                        //sDevice.AcquisitionSize++;
                                    }
                                    else
                                    {
                                        //Ocorreu erro ao tentar armazenar o registro
                                        sDevice.AcquisitionStatus = ACQ_FINISHED;
                                    }
                                    //
                                }
                            }
                        }
                    }
                    break;
                }
                //
                case ACQ_THR:
                {
                    //Armazenar a curva at� encontrar o Threshold final
                    if(StoreData() == 0)
                    {
                        //Verifica se � maior que anterior para salvar o pico
                        PeakDetect();
                        //Aponta para o pr�ximo registro
                        sDevice.AcquisitionIndex++;
                        //sDevice.AcquisitionSize++;
                    }
                    else
                    {
                        //Ocorreu erro ao tentar armazenar o registro
                        sDevice.AcquisitionStatus = ACQ_FINISHED;
                        break;
                    }
                    //
                    //Verifica se alcan�ou o Threshold final
                    if(sDevice.AcquisitionDirection == CW)
                    {
                        //Detectar o Threshold final positivo
                        if(lTempTorque <= sDevice.FinalThreshold)
                        {
                            //Detectado Threshold final
                            //Disparar timer e mudar de estado
                            //FINAL_THRESHOLD_TIMER
                            
                            if(sDevice.AcquisitionResetTimeout > 0)
                            {
                                TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionResetTimeout,RUN);
                                sDevice.AcquisitionStatus = ACQ_RESET_TIME;  
                            }
                            else
                            {
                                TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionFinalTimeout,RUN);
                                sDevice.AcquisitionStatus = ACQ_FINALTIMEOUT;
                                sDevice.AcquisitionTHFIndex = sDevice.AcquisitionIndex;
                            }
                            
                            
                        }
                    }
                    else
                    {
                        if(sDevice.AcquisitionDirection == CCW)
                        {
                            //Detectar o Threshold final negativo
                            lTemp = 0 - lTempTorque;
                            if(lTemp <= sDevice.FinalThreshold)
                            {
                                //Detectado Threshold final
                                //Disparar timer e mudar de estado
                                TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionFinalTimeout,RUN);
                                sDevice.AcquisitionStatus = ACQ_FINALTIMEOUT;
                                sDevice.AcquisitionTHFIndex = sDevice.AcquisitionIndex;
                            }
                        }
                    }
                    break;
                }
                //
                case ACQ_FINALTIMEOUT:
                {
                    //Verificar se atingiu o limite de tempo para encerrar
                    if(TimerStatus(FINAL_THRESHOLD_TIMER)==OVERFLOW)
                    {
                        //Atingiu o tempo limite
                        //Terminar aquisi��o
                        sDevice.AcquisitionStatus = ACQ_FINISHED;
                    }
                    else
                    {
                        //verfica se ultrapassou o Threshold final
                        //Caso sim, zerar timer
                        if(sDevice.AcquisitionDirection == CW)
                        {
                            if(lTempTorque >= sDevice.FinalThreshold)
                            {
                                //ultrapassou o Threshold final
                                //Parar timer
                                TimerStop(FINAL_THRESHOLD_TIMER);
                                //Mudar para o estado ACQ_THR
                                sDevice.AcquisitionStatus = ACQ_THR;
                            }
                        }
                        else
                        {
                            if(sDevice.AcquisitionDirection == CCW)
                            {
                                lTemp = 0 - lTempTorque;
                                if(lTemp >= sDevice.FinalThreshold)
                                {
                                    //ultrapassou o Threshold final
                                    //Parar timer
                                    TimerStop(FINAL_THRESHOLD_TIMER);
                                    //Mudar para o estado ACQ_THR
                                    sDevice.AcquisitionStatus = ACQ_THR;
                                }
                            }
                        }
                        //Continuar armazenando os registros
                        if(StoreData() == 0)
                        {
                            //Verifica se � maior que anterior para salvar o pico
                            PeakDetect();
                            //Aponta para o pr�ximo registro
                            sDevice.AcquisitionIndex++;
                            //sDevice.AcquisitionSize++;
                        }
                        else
                        {
                            //Ocorreu erro ao tentar armazenar o registro
                            sDevice.AcquisitionStatus = ACQ_FINISHED;
                            break;
                        }
                    }
                    break;
                }
                //
                case ACQ_FINISHED:
                {
                    //Aquisi��o finalizada
                    //
                    //Incrementa contador de ciclos

                    sDevice.AcquisitionDelayFlag = 0;
                    //Testa para verificar sobretorque
                    if(sDevice.MaxAppliedTorqueChecked == 0)
                    {
                        IncCycleCounter();
                        //testa se houve sobretorque
                        sDevice.MaxAppliedTorqueChecked = 1;
                        if(sDevice.AcquisitionDirection == CW)
                        {
                            if(sDevice.MaxAppliedTorque > sDevice.OverTorqueLimit)
                            {
                                //Ocorreu um sobretorque no sentido hor�rio
                                //Incrementar contador de sobretorque
                                IncMaxTorqueCounter();
                            }
                        }
                        else
                        {
                            if(sDevice.AcquisitionDirection == CCW)
                            {
                                lTemp = 0 - sDevice.MaxAppliedTorque;
                                if(lTemp > sDevice.OverTorqueLimit)
                                {
                                    //Ocorreu um sobretorque no sentido anti-hor�rio
                                    //Incrementar contador de sobretorque
                                    IncMaxTorqueCounter();
                                }
                            }
                        }
                    }
                    break;
                }
                //
                case ACQ_DELAY_TIME:
                {
                    if(TimerStatus(FINAL_THRESHOLD_TIMER)==OVERFLOW)
                    {
                        sDevice.AcquisitionDelayFlag = 1;
                        sDevice.AcquisitionStatus = ACQ_IDLE;
                    }
                    break;
                }
                //
                case ACQ_RESET_TIME:
                {
                    if(TimerStatus(FINAL_THRESHOLD_TIMER)==OVERFLOW)
                    {
                        TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionFinalTimeout,RUN);
                        sDevice.AcquisitionStatus = ACQ_FINALTIMEOUT;
                        sDevice.AcquisitionTHFIndex = sDevice.AcquisitionIndex;
                    }
                    break;
                }
                //
                default:
                {
                    break;
                }
                //
            }
                    //
        }
    }
}
 
 
 
 
 
 */