#include <stdio.h>
#include <stdlib.h>
//#include <libq.h>
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

void PeakCurve(void)
{
    if(sDevice.AnalogPowerState == ON)
    {
        lTempTorque = lADS1271Value;
        lTempAngle = sEncoder.PulseAcc;
        //A Alimentação analógica está ligada
        if(sDevice.AcquisitionReady == ACQ_READY)
        {
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
                            sDevice.AcquisitionStatus = ACQ_THR;
                            sDevice.AcquisitionDirection = CW;                            
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
                                //Threshold alcançado para o sentido esquerdo
                                
                                
                            }
                        }
                    }
                    break;
                }
                //
                case ACQ_THR:
                {
                    
                    break;
                }
                //
                case ACQ_FINALTIMEOUT:
                {
                    
                    break;
                }
                //
                case ACQ_FINISHED:
                {
                    
                    break;
                }
            }
        }
    }
}