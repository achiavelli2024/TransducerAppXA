#include <stdio.h>
#include <stdlib.h>
#include <libq.h>
#include "hardware.h"
#include "timer.h"
#include "ads1271.h"
#include "encoder.h"
#include "peakfinder.h"

long lTempClick;

extern long lTempAngle, lTempTorque, lTemp;
extern DeviceStruct sDevice, sTempDevice;
extern ClickStruct sTorqueClick;
extern DerivativeStruct sDerivativeTorque;

void PeakDetect(void);
void IncCycleCounter(void);
void IncMaxTorqueCounter(void);
unsigned char StoreData(void);
void TorqueClickDetector(void);

void ClickCurve(void)
{
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
                            sDerivativeTorque.Status = DERIVATE_NOT_READY;
                            sDerivativeTorque.Count = 0;  
                            sDerivativeTorque.MaxResult = 0; 
                            sDerivativeTorque.MinResult = 0;
                            lADS1271PrevValue = lTempTorque;
                            sDevice.FastScan = 0;
                            //Iniciar o Click Detector
                            sTorqueClick.Direction = STABLE;
                            sTorqueClick.CurrentValue = lTempTorque;
                            sTorqueClick.PrevValue = lTempTorque;
                            sTorqueClick.PeakValue = lTempTorque;
                            sTorqueClick.State = CLICK_SEARCH;
                            sTorqueClick.Count = 0;
                            //Início - Adicionado em 18/06/2025 por Reginaldo
                            if(sDevice.AcquisitionDelayTimeout > 0 && sDevice.AcquisitionDelayFlag == 0)
                            {
                                //O delay inicial estÃ¡ habilitado
                                TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionDelayTimeout,RUN);
                                sDevice.AcquisitionStatus = ACQ_DELAY_TIME;
                            }
                            else
                            {
                                sDevice.AcquisitionStatus = ACQ_THR;
                                sDevice.AcquisitionDirection = CW;
                            }                            
                            
                            //Mudar para próximo estado
                            //sDevice.AcquisitionStatus = ACQ_THR;
                            //sDevice.AcquisitionDirection = CW;
                            //fim
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
                                sDerivativeTorque.Status = DERIVATE_NOT_READY;
                                sDerivativeTorque.Count = 0;  
                                sDerivativeTorque.MaxResult = 0; 
                                sDerivativeTorque.MinResult = 0;
                                lADS1271PrevValue = lTempTorque;
                                //Iniciar o Click Detector
                                sTorqueClick.Direction = STABLE;
                                sTorqueClick.CurrentValue = lTempTorque;
                                sTorqueClick.PrevValue = lTempTorque;
                                sTorqueClick.PeakValue = lTempTorque;
                                sTorqueClick.State = CLICK_SEARCH;
                                sTorqueClick.Count = 0;
                                sDevice.FastScan = 0;
                                
                                //Início - Adicionado em 18/06/2025 por Reginaldo
                                if(sDevice.AcquisitionDelayTimeout > 0 && sDevice.AcquisitionDelayFlag == 0)
                                {
                                    //O delay inicial estÃ¡ habilitado
                                    TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionDelayTimeout,RUN);
                                    sDevice.AcquisitionStatus = ACQ_DELAY_TIME;
                                } 
                                else
                                {
                                    //Mudar para prÃ³ximo estado
                                    sDevice.AcquisitionStatus = ACQ_THR;
                                    sDevice.AcquisitionDirection = CCW;
                                }                                
                                
                                //Mudar para próximo estado
                                //sDevice.AcquisitionStatus = ACQ_THR;
                                //sDevice.AcquisitionDirection = CCW;
                                //fim
                            }
                        }
                    }
                    break;
                }
                //
                case ACQ_THR:
                {
                    //Verifica se é maior que anterior para salvar o pico
                    PeakDetect();
                    sTorqueClick.PrevValue = sTorqueClick.CurrentValue;
                    sTorqueClick.CurrentValue = lTempTorque;
                    
                    if(sDevice.AcquisitionDirection == CCW)
                    {
                        sTorqueClick.PrevValue = sTorqueClick.CurrentValue * (-1);
                        sTorqueClick.CurrentValue = lTempTorque * (-1);
                    }
                    
                    TorqueClickDetector();
                    if(sTorqueClick.State == CLICK_DETECTED)
                    {
                        sDevice.AcquisitionFirstPeakIndex = sDevice.AcquisitionPeakIndex;
                        sDevice.AcquisitionFirstPeak = sTorqueClick.PeakValue;//*****
                        sDevice.AcquisitionStatus = ACQ_FINALTIMEOUT;
                        TimerLoad(FINAL_THRESHOLD_TIMER,sDevice.AcquisitionFinalTimeout,RUN);                                
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
                        //
                        //ClickFind();
                        //sDevice.AcquisitionFirstPeak = sPeakFinder.ResultClickTorque;
                        //
                    }
                    else
                    {
                        //Verifica se é maior que anterior para salvar o pico
                        PeakDetect();
                    }
                    break;
                }
                //
                case ACQ_FINISHED:
                {
                    //Aquisição finalizada
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
                default:
                {

                    break;
                }
                //
            }
                    //
        }
    }
//*************************       
}
//
void TorqueClickDetector(void)
{
    unsigned char i;
    switch(sTorqueClick.State)
    {
        case CLICK_SEARCH:
        {
            if(sDevice.AcquisitionIndex > 0)
            {
                //eliminar a amostra mais antiga
                for(i = 0; i < (CLICK_SAMPLES-1); i++)
                {
                    sTorqueClick.Samples[CLICK_SAMPLES-1-i] = sTorqueClick.Samples[CLICK_SAMPLES-2-i];
                }
                //Salva a amostra atual
                sTorqueClick.Samples[0] = sTorqueClick.CurrentValue;                
                //
                if(sTorqueClick.CurrentValue > sTorqueClick.PeakValue)
                {
                    //Significa que o valor do torque continua subindo
                    sTorqueClick.PeakValue = sTorqueClick.CurrentValue;
                    sTorqueClick.SampleIndex = sDevice.AcquisitionIndex;
                }
                //
                if(sDevice.AcquisitionIndex > CLICK_SAMPLES+1)
                {
                    lTempClick = sTorqueClick.Samples[CLICK_SAMPLES-1] - sTorqueClick.CurrentValue;
                    if(lTempClick > 0)
                    {
                       //Se ï¿½ positivo, significa que o valor estï¿½ diminuindo 
                       //Computa o novo trigger 
                       sTorqueClick.Trigger = sTorqueClick.PeakValue / 100;
                       sTorqueClick.Trigger = sTorqueClick.Trigger * sDevice.ClickFall; 
                       if(sTorqueClick.CurrentValue <= sTorqueClick.Trigger)
                       {
                           //Detectou o click
                           sTorqueClick.State = CLICK_DETECTED;
                       }
                    }
                }
            }
            break;
        }
        //
        case CLICK_DETECTED:
        {
            //O clique foi detectado
            
            
            
            break;
        }        
    }
}

