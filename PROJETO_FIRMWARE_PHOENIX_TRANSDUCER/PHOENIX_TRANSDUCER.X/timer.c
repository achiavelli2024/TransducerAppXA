//
// T�tulo:  ITIB1 - Timers Handler
// Vers�o:  1.0.0
// Cria��o: 12/Mar�o/2013
// Lan�amento:
//
//                             DESCRI��O:
// Este arquivo cont�m o c�digo para manipula��o dos Timers da placa ITIB-1.
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
//  | 1.0.0    | 12/03/2013 | Reginaldo do Prado   | Cria��o do arquivo          |
//  +----------+------------+----------------------+-----------------------------+
//
//
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "tool.h"
#include "pwm.h"
//
//
Timer32bits sTimer[TIMERS_QUANTITY];
unsigned int uiAliveCounter;
//
//
extern ToolData sTool;
extern BuzzerStruct sBuzzer;
extern unsigned char ucFastCommState;
extern SerialRxBufferStruct sRxBuffer;
extern BlinkLedStruct sManutLed;
//
extern DeviceStruct sDevice;
//
long ADS1271SpiRead(void);
//
void TimerInit(void)
{
    uiAliveCounter = 0;
    //Inicializa Timer 1
    T1CONbits.TSIDL = 1;                //Desabilita timer em modo idle
    T1CONbits.TGATE = 0;                //Timer Gated Time Accumulation Desabilitado
    T1CONbits.TCKPS = 0b01;             //Prescaler = 8
    T1CONbits.TSYNC = 0;                //N�o sincroniza com clock externo
    T1CONbits.TCS = 0;                  //Timer alimentado pelo clock interno (Fosc/2)
    TMR1 = 0x0000;                      //Zera contador do timer
    PR1 = SYSTEMTIC-1;                  //Configura per�odo do timer
    T1CONbits.TON = 1;                  //Habilita Timer 1
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;

    //Inicializa Timer 2
    T2CONbits.TSIDL = 1;                //Desabilita timer em modo idle
    T2CONbits.TGATE = 0;                //Timer Gated Time Accumulation Desabilitado
    T2CONbits.TCKPS = 0b00;             //Prescaler = 8
    T2CONbits.T32 = 1;                  //16 bit Timer
    T2CONbits.TCS = 0;                  //Timer alimentado pelo clock interno (Fosc/2)
    TMR3 = 0x00;
    TMR2 = 0x00;
    PR3 = 0x0001;
    PR2 = 0x1170 -1;                    //Configura per�odo do timer
    T2CONbits.TON = 1;                  //Habilita timer

    //Inicializa Timer 4
    T4CONbits.TSIDL = 1;                //Desabilita timer em modo idle
    T4CONbits.TGATE = 0;                //Timer Gated Time Accumulation Desabilitado
    T4CONbits.TCKPS = 0b00;//0b10;             //Prescaler = 1
    T4CONbits.T32 = 0;                  //16 bit Timer
    T4CONbits.TCS = 0;                  //Timer alimentado pelo clock interno (Fosc/2)
    PR4 = FASTPWMTIC-1;                 //Configura per�odo do timer
    T4CONbits.TON = 1;                  //Habilita timer
//*********************
//    //Inicializa Timer 5
    T5CONbits.TSIDL = 1;                //Desabilita timer em modo idle
    T5CONbits.TGATE = 0;                //Timer Gated Time Accumulation Desabilitado
    T5CONbits.TCKPS = 0b01;             //Prescaler = 1
    T5CONbits.TCS = 0;                  //Timer alimentado pelo clock interno (Fosc/2)
    TMR5 = 0x0000;                      //Zera contador do timer
    PR5 = 65535;//(875*8)-1;                    //Configura per�odo do timer  (875*5)-1 = 500us; (875/2)-1 = 50us 
    T5CONbits.TON = 1;                  //Habilita timer
    IFS1bits.T5IF = 0;
    IEC1bits.T5IE = 0;
//***********************   
//    //Inicializa Timer 6
    T6CONbits.TSIDL = 1;                //Desabilita timer em modo idle
    T6CONbits.TGATE = 0;                //Timer Gated Time Accumulation Desabilitado
    T6CONbits.TCKPS = 0b01;             //Prescaler = 1
    T6CONbits.TCS = 0;                  //Timer alimentado pelo clock interno (Fosc/2)
    TMR6 = 0x0000;                      //Zera contador do timer
    PR6 = (875)-1;                    //Configura per�odo do timer 875
    T6CONbits.TON = 1;                  //Habilita timer
    IFS2bits.T6IF = 0;
    IEC2bits.T6IE = 1;    
}

//void Timer1InterruptHandler(void)
void __attribute__((__interrupt__,no_auto_psv)) _T1Interrupt(void)
{
    //Aqui � tratada a interrup��o gerada pelo Timer 1
    unsigned char ucTimerIndex;
    //Cada timer configurado � varrido
    uiAliveCounter++;
    if(uiAliveCounter >= 1000)
    {
        sDevice.Alive++;
        uiAliveCounter = 0;
    }
    for(ucTimerIndex=0;ucTimerIndex<TIMERS_QUANTITY;ucTimerIndex++)
    {
        if(sTimer[ucTimerIndex].State == RUN)           //Verifica se o timer est� habilitado
        {
            sTimer[ucTimerIndex].Counter--;             //Caso sim, decrementa
            if(sTimer[ucTimerIndex].Counter == 0)       //Verifica se zerou o contador
                sTimer[ucTimerIndex].State = OVERFLOW;  //Caso sim, altera status para OVERFLOW
        }
    }
    IFS0bits.T1IF = 0;                          //Zera flag de interrup��o
}
//
//
void __attribute__((__interrupt__,no_auto_psv)) _T5Interrupt(void)
{

    //TEST_PIN = ~TEST_PIN;
    
    IFS1bits.T5IF = 0;
    /*
    ADC_SYNC_PIN = OFF;
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
    Nop(); 
    Nop();   
    Nop(); 
    Nop();   
    Nop();     
    ADS1271SpiRead();
    */
    
}
//
void __attribute__((__interrupt__,no_auto_psv)) _T6Interrupt(void)
{
    //T5CONbits.TON = 0;                      //desabilita timer
    //sRxBuffer.Retries++;                    //Incrementa contador de Retries
    sDevice.AcqTrigger = 1;
    //PORTFbits.RF1 = ~PORTFbits.RF1;
    IFS2bits.T6IF = 0;
}
//
//
void SoftTimerInit(void)
{
    //Inicializa todos os timers de software com o status STOP e valor de contagem em 2^32
    //
    unsigned char ucIndex;
    for(ucIndex=0;ucIndex<TIMERS_QUANTITY;ucIndex++)
    {
        sTimer[ucIndex].State = STOP;
        sTimer[ucIndex].Counter = 0xFFFFFFFF;
    }
}
//
void TimerStart(unsigned char ucTimer)
{
    //Dispara timer indicado, iniciando a contagem no valor corrente
    sTimer[ucTimer].State = RUN;
}
//
void TimerStop(unsigned char ucTimer)
{
    //Para o Timer indicado. O valor da contagem n�o � alterado
    sTimer[ucTimer].State = STOP;
}
//
void TimerClear(unsigned char ucTimer)
{
    //Para o Timer indicado e altera o valor do contador para o m�ximo (2^32)
    sTimer[ucTimer].Counter = 0xFFFFFFFF;
    sTimer[ucTimer].State = STOP;
}
//
void TimerLoad(unsigned char ucTimer, unsigned long ulCounter, unsigned char ucState)
{
    //Carrega o timer indicado com o valor desejado
    //Tamb�m � poss�vel disparar o timer com a nova contagem
    sTimer[ucTimer].Counter = ulCounter;
    sTimer[ucTimer].State = ucState;
}
//
unsigned char TimerStatus(unsigned char ucTimer)
{
    //Retorna o estado do timer indicado
    return sTimer[ucTimer].State;
}
//
unsigned long TimerCount(unsigned char ucTimer)
{
    //Retorna o valor de contagem do timer indicado
    return sTimer[ucTimer].Counter;
}
//
void TimerRefresh(void)
{


    
}
//
void TimerDelay(unsigned char timer, unsigned long value)
{
    TimerLoad(timer,value,RUN);         
    while(TimerStatus(timer)==RUN)
        PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
}