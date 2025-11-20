#include <stdio.h>
#include <stdlib.h>
#include <libq.h>
#include "hardware.h"
#include "timer.h"
#include "ads1271.h"
#include "encoder.h"

extern long lTempAngle, lTempTorque, lTemp;
extern DeviceStruct sDevice, sTempDevice;
extern ClickStruct sTorqueClick;
extern DerivativeStruct sDerivativeTorque;

void PeakDetect(void);
void IncCycleCounter(void);
void IncMaxTorqueCounter(void);
unsigned char StoreData(void);
void TorqueClickDetector(void);

void ShutOffCurve(void)
{
    sDevice.AcquisitionResetTimeout = 300;
    sDevice.AcquisitionDelayTimeout = ACQ_DELAY_DEFAULT;
    if(sDevice.AnalogPowerState == ON)
    {
        lTempTorque = lADS1271Value;
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
                            //Zera ângulo
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
                                //if(StoreData() == 0)
                                //{   
                                    //Aponta para o próximo registro
                                    //sDevice.AcquisitionIndex++;
                                    //Mudar para próximo estado
                                    sDevice.AcquisitionStatus = ACQ_THR;
                                    sDevice.AcquisitionDirection = CW;
                                    //sDevice.AcquisitionSize++;
                                //}
                                //else
                                //{
                                    //Ocorreu erro ao tentar armazenar o registro
                                //    sDevice.AcquisitionStatus = ACQ_FINISHED;
                                //}
                                //***
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
                                    //if(StoreData() == 0)
                                    //{   
                                        //Aponta para o próximo registro
                                    //    sDevice.AcquisitionIndex++;
                                        //Mudar para próximo estado
                                        sDevice.AcquisitionStatus = ACQ_THR;
                                        sDevice.AcquisitionDirection = CCW;
                                        //sDevice.AcquisitionSize++;
                                    //}
                                    //else
                                    //{
                                        //Ocorreu erro ao tentar armazenar o registro
                                    //    sDevice.AcquisitionStatus = ACQ_FINISHED;
                                    //}
                                    //****
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
                    //if(StoreData() == 0)
                    //{
                        //Verifica se é maior que anterior para salvar o pico
                        PeakDetect();
                        //Aponta para o próximo registro
                        //sDevice.AcquisitionIndex++;
                        //sDevice.AcquisitionSize++;
                    //}
                    //else
                    //{
                        //Ocorreu erro ao tentar armazenar o registro
                    //    sDevice.AcquisitionStatus = ACQ_FINISHED;
                    //    break;
                    //}
                    //
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
                        //Continuar armazenando os registros
                        //if(StoreData() == 0)
                        //{
                            //Verifica se é maior que anterior para salvar o pico
                            PeakDetect();
                            //Aponta para o próximo registro
                        //    sDevice.AcquisitionIndex++;
                            //sDevice.AcquisitionSize++;
                        //}
                        //else
                        //{
                            //Ocorreu erro ao tentar armazenar o registro
                        //    sDevice.AcquisitionStatus = ACQ_FINISHED;
                        //    break;
                        //}
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
