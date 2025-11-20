#include <stdio.h>
#include <stdlib.h>
#include <libq.h>
#include "hardware.h"
#include "timer.h"
#include "peakfinder.h"

extern DeviceStruct sDevice;
PeakFinderStruct sPeakFinder;
unsigned int uiIndex;
unsigned char Trq0, Trq1, Trq2; 
long lDerivativeTemp, ltemp, ltemp2, ltemp3, ltemp4, ltemp5;
unsigned int iloop, jloop, itemp, uiCounter, kloop, TempPeakIndex;

#define FPEAKLIST_SIZE      10
float fPeakList[FPEAKLIST_SIZE] = {0};

long TorqueRegistry2Long(unsigned int RegistryAddress);

void PeakFinderInit(void)
{
    sPeakFinder.SmoothCoef[0] = 0;
    sPeakFinder.SmoothCoef[1] = 0;
    sPeakFinder.SmoothCoef[2] = 0;
    sPeakFinder.SmoothCoef[3] = -4;
    sPeakFinder.SmoothCoef[4] = -3;
    sPeakFinder.SmoothCoef[5] = -3;
    sPeakFinder.SmoothCoef[6] = -2;
    sPeakFinder.SmoothCoef[7] = -1;
    sPeakFinder.SmoothCoef[8] = 0;
    sPeakFinder.SmoothCoef[9] = 1;
    sPeakFinder.SmoothCoef[10] = 2;
    sPeakFinder.SmoothCoef[11] = 3;
    sPeakFinder.SmoothCoef[12] = 3;
    sPeakFinder.SmoothCoef[13] = 4;
    sPeakFinder.SmoothCoef[14] = 0;
    sPeakFinder.SmoothCoef[15] = 0;
    sPeakFinder.SmoothCoef[16] = 0;    
    sPeakFinder.ValueThreshold = 150000;//673625;
    //
    sPeakFinder.DerivativeTheshold = 35000;//36446;
}
//
void PeakFinder(unsigned int DataLenght)
{
    sPeakFinder.DerivativePrevReady = 0;
    sPeakFinder.DerivativeThesholdCount = 0;
    sPeakFinder.ValueThresholdCount = 0;
    sPeakFinder.ZeroCrossCount = 0;
    sPeakFinder.DerivativePrevious = 0;
    sPeakFinder.DerivativeCurrent = 0;
    sPeakFinder.PeakCounter = 0;
    //Verifica se h? registros suficientes para an?lise 
    if(DataLenght > MIN_DATABASE_SIZE)
    {
        lDerivativeTemp = 0;
        for(iloop = SMOOTH_COEF_SIZE; iloop < (DataLenght - SMOOTH_COEF_SIZE); iloop++)
        {
            ClrWdt();
            lDerivativeTemp = 0;
            ltemp = TorqueRegistry2Long(iloop);
            if(ltemp >= sPeakFinder.ValueThreshold)
            {
                //Se for maior que o threshold, calcula a derivada
                sPeakFinder.ValueThresholdCount++;
                for(jloop = 0; jloop < (SMOOTH_COEF_SIZE * 2 + 1); jloop++)
                {
                    uiIndex = iloop - SMOOTH_COEF_SIZE + jloop;
                    ltemp = TorqueRegistry2Long(uiIndex);// * (-1); //teste negativo
                    ltemp2 = ltemp * sPeakFinder.SmoothCoef[jloop];
                    lDerivativeTemp += ltemp2;
                    PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                }
                sPeakFinder.DerivativePrevious = sPeakFinder.DerivativeCurrent;
                sPeakFinder.DerivativeCurrent = lDerivativeTemp;        //Salva derivada corrente
                //Verifica se é o primeiro registro analisado
                if(sPeakFinder.DerivativePrevReady == 1)
                {
                    //Não é o primeiro registro. A comparação com o anterior pode ser feita
                    //verifica se atingiu o threshold da derivada (inclinação)
                    sPeakFinder.DerivativeDelta = sPeakFinder.DerivativePrevious - sPeakFinder.DerivativeCurrent;
                    if(sPeakFinder.DerivativeDelta >= sPeakFinder.DerivativeTheshold)
                    {
                        ClrWdt();
                        //A inclinação é maior que o threshold
                        sPeakFinder.DerivativeThesholdCount++;
                        //Verificar se a derivada passa pelo zero (do positivo para o negativo)
                        if((sPeakFinder.DerivativePrevious >=0) && (sPeakFinder.DerivativeCurrent < 0))
                        {
                            sPeakFinder.ZeroCrossCount++;
                            //Esse ponto atende as exigências:
                            //1: Value > ValueThreshold
                            //2: Slope > SlopeThreshold
                            //3: Derivada passou pelo zero
                            //
                            //Este ponto é um pico
                            //Salva index do registro onde o pico foi detectado
                            //Verifica se há espaço para salvar o index
                            if(sPeakFinder.PeakCounter < MAX_PEAK_COUNT)
                            {
                                //****************
                                ltemp4 = TorqueRegistry2Long(iloop);
                                TempPeakIndex = iloop;
                                for(kloop = 0; kloop <= (2*PEAK_PHASE_SIZE); kloop++)
                                {
                                    ltemp5 = TorqueRegistry2Long(iloop + kloop - PEAK_PHASE_SIZE);
                                    if(ltemp5 > ltemp4)
                                    {
                                        ltemp4 = ltemp5;
                                        TempPeakIndex = iloop + kloop - PEAK_PHASE_SIZE;
                                    }
                                }
                                //****************
                                sPeakFinder.PeakIndexList[sPeakFinder.PeakCounter] = TempPeakIndex;//iloop - 1;                            
                                //Incrementa contador de picos
                                sPeakFinder.PeakCounter++;
                            }
                            else
                            {
                                //Não há espaço para armazenar o index

                            }
                        }
                    }
                }
                else
                {
                    //Salva a primeira an?lise como valor pr?vio
                    sPeakFinder.DerivativePrevious = sPeakFinder.DerivativeCurrent;
                    //Zera contador de picos
                    sPeakFinder.PeakCounter = 0;
                    //Habilita compara??o na pr?xima passagem.
                    sPeakFinder.DerivativePrevReady = 1;
                }
            }
        }
    }
    else
    {
        //N?o h? registros suficiente para an?lise

    }
}
//
unsigned int GetMaxValueIndex(unsigned int DataLenght)
{
    unsigned int i;
    unsigned int j = 0;
    long MaxValue = -100000000;
    long TempValue;
    for(i=0; i< DataLenght; i++)
    {
        TempValue = TorqueRegistry2Long(i);
        if(TempValue > MaxValue)
        {
            MaxValue = TempValue;
            j = i;
        }
    }
    return j;
}
unsigned int GetMaxIndex(unsigned int IndexLenght)
{
    unsigned int i, index;
    long TempValue = -100000000;
    
    for(i = 0; i< IndexLenght; i++)
    {
        if(TorqueRegistry2Long(sPeakFinder.PeakIndexList[i]) > TempValue)
        {
            TempValue = TorqueRegistry2Long(sPeakFinder.PeakIndexList[i]);
            index = i;
        }
    }
    return index;
}
//
unsigned int GetMinValueIndex(unsigned int DataLenght, long OffSet)
{
    unsigned int i;
    unsigned int j = 0;
    long MinValue = 100000000;
    long TempValue;
    for(i=0; i< DataLenght; i++)
    {
        TempValue = TorqueRegistry2Long(i);

    }
}
//
unsigned int GetClickIndex(unsigned int DataLenght)
{
    unsigned int i;
    unsigned int j = 0;
    unsigned int k = 0;
    long MaxValue = -100000000;
    long MinValue = 100000000;
    long TempValue;
    for(i = 0; i < DataLenght; i++)
    {
        TempValue = TorqueRegistry2Long(i);
        if(TempValue >= MaxValue)
        {
            MaxValue = TempValue;
            j = i;
        }
        else if(TempValue < MinValue)
        {
            MinValue = TempValue;
            k = i;
        }
    }

    return 0;
}
//
void GetPeak(void)
{
    unsigned int i, index;
    long CurrentValue, Peak;
    CurrentValue = TorqueRegistry2Long(sPeakFinder.PeakIndexList[0]);
    Peak = CurrentValue;
    index = 0;
    for(i = 1; i < sPeakFinder.PeakCounter; i++)
    {
        CurrentValue = TorqueRegistry2Long(sPeakFinder.PeakIndexList[i]);
        if(CurrentValue > Peak)
            index = i;
    }
    //sPeakFinder.PeakIndexList[0] = sPeakFinder.PeakIndexList[index];
    //sPeakFinder.PeakCounter = 1;
    sPeakFinder.PeakIndexList[sPeakFinder.PeakCounter] = sPeakFinder.PeakIndexList[index];
    sPeakFinder.PeakCounter++;
}
//
void PeakList2Float(void)
{
    unsigned i = 0;
    long CurrentValue = 0;
    double temp;
    for(i=0; i< FPEAKLIST_SIZE; i++)
    {
       fPeakList[i] = 0.0;  
    }
    if(sPeakFinder.PeakCounter > FPEAKLIST_SIZE)
    {
        for(i=0; i< FPEAKLIST_SIZE; i++)
        {
            CurrentValue = TorqueRegistry2Long(sPeakFinder.PeakIndexList[i]);
            temp = (double)CurrentValue;
            temp =  temp * (float)sDevice.TorqueSensitivity;
            temp = temp / 1000000000000; 
            fPeakList[i] = temp;      
        }       
    }
    else
    {
        for(i=0; i< sPeakFinder.PeakCounter; i++)
        {
            CurrentValue = TorqueRegistry2Long(sPeakFinder.PeakIndexList[i]);
            temp = (double)CurrentValue;
            temp =  temp * (float)sDevice.TorqueSensitivity;
            temp = temp / 1000000000000; 
            fPeakList[i] = temp;      
        }
    }
}
//
void ClickFind(void)
{
    unsigned int MaxIndex, MaxPeakIndex;
    long Value1, Value2;
    MaxIndex = GetMaxValueIndex(sDevice.AcquisitionSize);
    sPeakFinder.ValueThreshold = (TorqueRegistry2Long(MaxIndex)/10) * 5;
    sPeakFinder.DerivativeTheshold = 10000;//25000;//50000;//36446;
    PeakFinder(sDevice.AcquisitionSize);
    
    if(sPeakFinder.PeakCounter == 0)
        sPeakFinder.ResultClickTorque = sDevice.AcquisitionMaxTorque;//sClickWrench.MaxTorqueValue;
    else if(sPeakFinder.PeakCounter == 1)
    {
        sPeakFinder.ResultClickTorque = TorqueRegistry2Long(sPeakFinder.PeakIndexList[sPeakFinder.PeakCounter]);//-1
    }
    else if (sPeakFinder.PeakCounter == 2)
    {
        Value1 = TorqueRegistry2Long(sPeakFinder.PeakIndexList[0]);
        Value2 = TorqueRegistry2Long(sPeakFinder.PeakIndexList[1]);
        if(Value1 <= Value2)
        {
            sPeakFinder.ResultClickTorque = Value1;
        }
        else 
        {
            sPeakFinder.ResultClickTorque = Value2;
            sPeakFinder.PeakIndexList[0] = sPeakFinder.PeakIndexList[1];  
        }
        sPeakFinder.PeakCounter = 1;
    }
    else if(sPeakFinder.PeakCounter >= 3)
    {
        MaxPeakIndex = GetMaxIndex(sPeakFinder.PeakCounter);
        if(MaxPeakIndex > 0)
        {
            sPeakFinder.ResultClickTorque = TorqueRegistry2Long(sPeakFinder.PeakIndexList[MaxPeakIndex - 1]);
            sPeakFinder.PeakIndexList[0] = sPeakFinder.PeakIndexList[MaxPeakIndex - 1];
            sPeakFinder.PeakCounter = 1;
        }
        else
        {
            sPeakFinder.ResultClickTorque = TorqueRegistry2Long(sPeakFinder.PeakIndexList[MaxPeakIndex]);
            sPeakFinder.PeakCounter = 1;
        }
    }
}
