//
// T�tulo:  PHOENIX TRANSDUCER - ADS1271 Handler
// Vers�o:  1.0.0
// Cria��o: 29/Julho/2016
// Lan�amento:
//
//                             DESCRI��O:
// Este arquivo cont�m o c�digo para manipula��o dos canais anal�gicos do ADS1271.
// Escrito para o PIC24EP512GP806 e compilador XC16.
//
// Copyright (C) 2013 M.Shimizu El�trica e Pneum�tica Ltda. Todos os direitos reservados.
//
//                            AVISO LEGAL:
//
// Este arquivo e todo seu conte�do � propriedade da M.Shimizu El�trica e Pneum�tica Ltda.
// A publica��o, distribui��o ou modifica��es, totais or parciais, s�o expressamente proibidas
// sem autori��o da M.Shimizu ou dos seus representantes legais.
//
// Para maiores informa��es viste www.mshimizu.com.br
//
//  +----------+------------+----------------------+-----------------------------+
//  | Vers�o   | Data       | Autor                | Coment�rio                  |
//  +----------+------------+----------------------+-----------------------------+
//  | 1.0.0    | 27/07/2016 | Reginaldo do Prado   | Cria��o do arquivo          |
//  +----------+------------+----------------------+-----------------------------+
//
//

//http://e2e.ti.com/support/data_converters/precision_data_converters/f/73/p/385240/1359047



#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "spi.h"
#include "timer.h"
#include "pwm.h"
#include "butterworth.h"

long lTempADC = 0;
long lValueADC = 0;
long lTorqueOffset = 0;
MovAverageStruct sMovAverage;
double AdValueNm = 0;

double dTemp, dTemp2;
long lADS1271Value = 0;
long lADS1271PrevValue = 0;
long lADS1271Data[OFFSET_SAMPLES];
long lADS1271ValueMax = 0;
long lADS1271ValueMin = 0;
unsigned char lADS1271SampleIndex = 0;
unsigned char ucADS1271ConvReady = 0;
unsigned char ucADS1271ValueReady = 0;
extern DeviceStruct sDevice;
extern ButterStruct sButter;
//
void ADS1271ClockEnable(unsigned char state);
long ADS1271SpiRead(void);
//
void ADS1271SetClock(void)
{
    PwmOff(ADC_CLOCK_OC);
    TimerLoad(DELAYTIMER,50,RUN);
    sDevice.FilterPeriod = sDevice.FilterFreq;
    sDevice.FilterPeriod = sDevice.FilterPeriod * 1046;
    sDevice.FilterPeriod = sDevice.FilterPeriod / FILTER_PWM_STEP;
    sDevice.FilterPwm = (unsigned int)(1000000 / sDevice.FilterPeriod);
    PwmInit(ADC_CLOCK_OC,PWM_FAST_SOURCE,sDevice.FilterPwm/2 ,sDevice.FilterPwm,PWM_ON);
}
//
void ADS1271Init(void)
{
    //ADS1271ClockEnable(ON);         //Habilita gera��o do clock pelo PIC
    sDevice.FilterFreq = FILTER_FREQ_DEFAULT;
    ADS1271SetClock();
    //ADS1271ClockEnable(0);   //Clock para m�xima amostragem
    //Ap�s iniciar o clock para o ADS, aguardar estabiliza��o antes de habilitar a SPI
    //Datasheet(Novembro 2004) p�gina 25
    //" After Power On, SCLK remains an output until a few clocks have been received on the CLK input."
    TimerLoad(DELAYTIMER,50,RUN);
    //
    ADC_SYNC_PIN = ON;              //SYNC = 0 = desabilitado
    ADC_FORMAT_PIN = ON;           //FORMAT = 0 = SPI
    ADC_MODE_PIN = OFF;             //MODE = 0 = High Speed 
    while(TimerStatus(DELAYTIMER)==RUN); 
    SpiInit(1);                     //Inicializa canal SPI
    INTCON2bits.INT1EP = 1;         //Interrup��o INT1 na borda de subida = 0
    IEC1bits.INT1IE = 1;            //Habilita interrup��o de INT1
    sMovAverage.Pointer = 0;
    sMovAverage.Result = 0;
}
//
void ADS1271ClockEnable(unsigned char state)
{
    REFOCONbits.ROON = 0;
    REFOCONbits.ROSEL = 1;
    //REFOCONbits.RODIV = 0b0100;
    REFOCONbits.RODIV = 0b0011;//0b0000;
    if(state == 1)
        REFOCONbits.ROON = 1;
    else
        REFOCONbits.ROON = 0;
}
//
void ADS1271StartConv(void)
{
    //Syncroniza o ADS e inicia convers�o
    ADC_SYNC_PIN = ON;
    Nop();
    Nop();
    //ADC_SYNC_PIN = OFF;
    Nop();
    Nop();   
    Nop();
    Nop(); 
    Nop();
    Nop();   
    Nop();
    Nop();
    Nop();
    Nop();   
    Nop();
    Nop(); 
    Nop();
    Nop();   
    Nop();
    Nop(); 
    Nop();
    Nop();   
    Nop();
    Nop(); 
    Nop();
    Nop();   
    Nop();
    Nop();     
    
    ADC_SYNC_PIN = ON;
    Nop();
    Nop();    
}
//
void ADS1271InterruptHandler(void)
{
    IFS1bits.INT1IF = 0;
    ucADS1271ConvReady = 2;
    SPI1BUF = 0x00; //Dummy byte apenas para gera��o do clock
    while(!SPI1STATbits.SPIRBF);
    lTempADC = SPI1BUF;
    lValueADC = lTempADC << 16;
    //Obt�m byte 2
    SPI1BUF = 0x00; //Dummy byte apenas para gera��o do clock
    while(!SPI1STATbits.SPIRBF);
    lTempADC = SPI1BUF;
    lValueADC |= lTempADC << 8;
    //Obt�m byte menos significativo
    SPI1BUF = 0x00; //Dummy byte apenas para gera��o do clock
    while(!SPI1STATbits.SPIRBF);
    lTempADC = SPI1BUF;
    lValueADC |= lTempADC;  
    if(lValueADC > 0x7FFFFF)
        lValueADC |= 0xFF000000; 
    //Aplica o filtro
    xData[3] = xData[2];
    xData[2] = xData[1];
    xData[1] = xData[0];
    xData[0] = (double)lValueADC;
    yData[3] = yData[2];
    yData[2] = yData[1];
    yData[1] = yData[0];
    dTemp = sButter.B0A0 * xData[0];
    dTemp += sButter.B1A0 * xData[1];
    dTemp += sButter.B2A0 * xData[2];
    dTemp += sButter.B3A0 * xData[3];
    dTemp2 = sButter.A1A0 * yData[1];
    dTemp2 += sButter.A2A0 * yData[2];
    dTemp2 += sButter.A3A0 * yData[3];
    yData[0] = dTemp - dTemp2;
    lADS1271Value = (long)yData[0];
}
//
long ADS1271SpiRead(void)
{
    //TEST_PIN = ON;
    long lTemp = 0;
    long lValue = 0;
    //Obt�m byte mais significativo
    SPI1BUF = 0x00; //Dummy byte apenas para gera��o do clock
    while(!SPI1STATbits.SPIRBF);
    lTemp = SPI1BUF;
    lValue = lTemp << 16;
    //Obt�m byte 2
    SPI1BUF = 0x00; //Dummy byte apenas para gera��o do clock
    while(!SPI1STATbits.SPIRBF);
    lTemp = SPI1BUF;
    lValue |= lTemp << 8;
    //Obt�m byte menos significativo
    SPI1BUF = 0x00; //Dummy byte apenas para gera��o do clock
    while(!SPI1STATbits.SPIRBF);
    lTemp = SPI1BUF;
    lValue |= lTemp;  
    lValue &= 0xFFFFFFF8;
    //IEC1bits.INT1IE = 1;
    //TEST_PIN = OFF;
    return lValue;
}
