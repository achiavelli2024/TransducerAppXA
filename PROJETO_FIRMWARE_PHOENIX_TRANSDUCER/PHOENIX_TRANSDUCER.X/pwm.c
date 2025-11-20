//
// T�tulo:  ITIB1 - PWM Handler
// Vers�o:  1.0.0
// Cria��o: 12/Mar�o/2013
// Lan�amento:
//
//                             DESCRI��O:
// Este arquivo cont�m o c�digo para manipula��o dos PWMs da placa ITIB-1.
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
//  | Versão   | Data       | Autor                | Comentário                  |
//  +----------+------------+----------------------+-----------------------------+
//  | 1.0.0    | 25/03/2013 | Reginaldo do Prado   | Criação do arquivo          |
//  +----------+------------+----------------------+-----------------------------+
//
//
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
//
void PwmInit(unsigned char ucChannel, unsigned char ucSource, unsigned int uiOnTime, unsigned int uiPeriod, unsigned char ucMode)
{
    if(ucChannel == 1)
    {
        OC1CON1 = 0;
        OC1CON2 = 0;
        OC1CON1bits.OCSIDL = 1;                 //Desabilita em modo idle
        OC1CON1bits.OCTSEL = ucSource;          //Clock source = T2CLK
        OC1CON2bits.FLTOUT = 0;                 //0 =PWM output is driven low on a Fault
        OC1CON2bits.FLTTRIEN = 0;               //OCx pin I/O state defined by FLTOUT bit on Fault condition
        OC1CON2bits.OCINV = 0;                  //0 =OCx output is not inverted
        OC1CON2bits.OC32 = 0;                   //Cascade module operation disabled
        OC1R = uiOnTime;
        OC1RS = uiPeriod;
        OC1CON1bits.OCM = ucMode;               //Modo de operação do PWM
        OC1CON2bits.SYNCSEL = 0x1F;             //No trigger or sync source is selected
//        OC1CON2bits.OCTRIS = ucMode;
    }
    if(ucChannel == 2)
    {
        OC2CON1 = 0;
        OC2CON2 = 0;
        OC2CON1bits.OCSIDL = 1;                 //Desabilita em modo idle
        OC2CON1bits.OCTSEL = ucSource;          //Clock source = T2CLK
        OC2CON2bits.FLTOUT = 0;                 //0 =PWM output is driven low on a Fault
        OC2CON2bits.FLTTRIEN = 0;               //OCx pin I/O state defined by FLTOUT bit on Fault condition
        OC2CON2bits.OCINV = 0;                  //0 =OCx output is not inverted
        OC2CON2bits.OC32 = 0;                   //Cascade module operation disabled
        OC2R = uiOnTime;
        OC2RS = uiPeriod;
        OC2CON1bits.OCM = ucMode;               //Modo de opera��o do PWM
        OC2CON2bits.SYNCSEL = 0x1F;             //No trigger or sync source is selected
//        OC2CON2bits.OCTRIS = ucMode;
    }
    if(ucChannel == 3)
    {
        OC3CON1 = 0;
        OC3CON2 = 0;
        OC3CON1bits.OCSIDL = 1;                 //Desabilita em modo idle
        OC3CON1bits.OCTSEL = ucSource;          //Clock source = T2CLK
        OC3CON2bits.FLTOUT = 0;                 //0 =PWM output is driven low on a Fault
        OC3CON2bits.FLTTRIEN = 0;               //OCx pin I/O state defined by FLTOUT bit on Fault condition
        OC3CON2bits.OCINV = 0;                  //0 =OCx output is not inverted
        OC3CON2bits.OC32 = 0;                   //Cascade module operation disabled
        OC3R = uiOnTime;
        OC3RS = uiPeriod;
        OC3CON1bits.OCM = ucMode;               //Modo de opera��o do PWM
        OC3CON2bits.SYNCSEL = 0x1F;             //No trigger or sync source is selected
//        OC3CON2bits.OCTRIS = ucMode;
    }
    if(ucChannel == 4)
    {
        OC4CON1 = 0;
        OC4CON2 = 0;
        OC4CON1bits.OCSIDL = 1;                 //Desabilita em modo idle
        OC4CON1bits.OCTSEL = ucSource;          //Clock source = T2CLK
        OC4CON2bits.FLTOUT = 0;                 //0 =PWM output is driven low on a Fault
        OC4CON2bits.FLTTRIEN = 0;               //OCx pin I/O state defined by FLTOUT bit on Fault condition
        OC4CON2bits.OCINV = 0;                  //0 =OCx output is not inverted
        OC4CON2bits.OC32 = 0;                   //Cascade module operation disabled
        OC4R = uiOnTime;
        OC4RS = uiPeriod;
        OC4CON1bits.OCM = ucMode;               //Modo de opera��o do PWM
        OC4CON2bits.SYNCSEL = 0x1F;             //No trigger or sync source is selected
//        OC4CON2bits.OCTRIS = ucMode;
    }
    if(ucChannel == 5)
    {
        OC5CON1 = 0;
        OC5CON2 = 0;
        OC5CON1bits.OCSIDL = 1;                 //Desabilita em modo idle
        OC5CON1bits.OCTSEL = ucSource;          //Clock source = T2CLK
        OC5CON2bits.FLTOUT = 0;                 //0 =PWM output is driven low on a Fault
        OC5CON2bits.FLTTRIEN = 0;               //OCx pin I/O state defined by FLTOUT bit on Fault condition
        OC5CON2bits.OCINV = 0;                  //0 =OCx output is not inverted
        OC5CON2bits.OC32 = 0;                   //Cascade module operation disabled
        OC5R = uiOnTime;
        OC5RS = uiPeriod;
        OC5CON1bits.OCM = ucMode;               //Modo de opera��o do PWM
        OC5CON2bits.SYNCSEL = 0x1F;             //No trigger or sync source is selected
//        OC5CON2bits.OCTRIS = ucMode;
    }
    if(ucChannel == 6)
    {
        OC6CON1 = 0;
        OC6CON2 = 0;
        OC6CON1bits.OCSIDL = 1;                 //Desabilita em modo idle
        OC6CON1bits.OCTSEL = ucSource;          //Clock source = T2CLK
        OC6CON2bits.FLTOUT = 0;                 //0 =PWM output is driven low on a Fault
        OC6CON2bits.FLTTRIEN = 0;               //OCx pin I/O state defined by FLTOUT bit on Fault condition
        OC6CON2bits.OCINV = 0;                  //0 =OCx output is not inverted
        OC6CON2bits.OC32 = 0;                   //Cascade module operation disabled
        OC6R = uiOnTime;
        OC6RS = uiPeriod;
        OC6CON1bits.OCM = ucMode;               //Modo de opera��o do PWM
        OC6CON2bits.SYNCSEL = 0x1F;             //No trigger or sync source is selected
//        OC6CON2bits.OCTRIS = ucMode;
    }

    if(ucChannel == 7)
    {
        OC7CON1 = 0;
        OC7CON2 = 0;
        OC7CON1bits.OCSIDL = 1;                 //Desabilita em modo idle
        OC7CON1bits.OCTSEL = ucSource;          //Clock source = T2CLK
        OC7CON2bits.FLTOUT = 0;                 //0 =PWM output is driven low on a Fault
        OC7CON2bits.FLTTRIEN = 0;               //OCx pin I/O state defined by FLTOUT bit on Fault condition
        OC7CON2bits.OCINV = 0;                  //0 =OCx output is not inverted
        OC7CON2bits.OC32 = 0;                   //Cascade module operation disabled
        OC7R = uiOnTime;
        OC7RS = uiPeriod;
        OC7CON1bits.OCM = ucMode;               //Modo de opera��o do PWM
        OC7CON2bits.SYNCSEL = 0x1F;             //No trigger or sync source is selected
//        OC7CON2bits.OCTRIS = ucMode;
    }
    if(ucChannel == 8)
    {
        OC8CON1 = 0;
        OC8CON2 = 0;
        OC8CON1bits.OCSIDL = 1;                 //Desabilita em modo idle
        OC8CON1bits.OCTSEL = ucSource;          //Clock source = T2CLK
        OC8CON2bits.FLTOUT = 0;                 //0 =PWM output is driven low on a Fault
        OC8CON2bits.FLTTRIEN = 0;               //OCx pin I/O state defined by FLTOUT bit on Fault condition
        OC8CON2bits.OCINV = 0;                  //0 =OCx output is not inverted
        OC8CON2bits.OC32 = 0;                   //Cascade module operation disabled
        OC8R = uiOnTime;
        OC8RS = uiPeriod;
        OC8CON1bits.OCM = ucMode;               //Modo de opera��o do PWM
        OC8CON2bits.SYNCSEL = 0x1F;             //No trigger or sync source is selected
        //OC8CON2bits.OCTRIS = 0;//ucMode;
    }
}

void PwmOn(unsigned char ucChannel)
{
    if(ucChannel == 1)
        OC1CON1bits.OCM = PWM_ON;
    if(ucChannel == 2)
        OC2CON1bits.OCM = PWM_ON;
    if(ucChannel == 3)
        OC3CON1bits.OCM = PWM_ON;
    if(ucChannel == 4)
        OC4CON1bits.OCM = PWM_ON;
    if(ucChannel == 5)
        OC5CON1bits.OCM = PWM_ON;
    if(ucChannel == 6)
        OC6CON1bits.OCM = PWM_ON;
    if(ucChannel == 7)
        OC7CON1bits.OCM = PWM_ON;
    if(ucChannel == 8)
        OC8CON1bits.OCM = PWM_ON;
}
//
void PwmOff(unsigned char ucChannel)
{
    if(ucChannel == 1)
    {
        //OC1CON2bits.OCTRIS = 1;
        OC1CON1bits.OCM = PWM_OFF;
    }
    if(ucChannel == 2)
    {
        //OC2CON2bits.OCTRIS = 1;
        OC2CON1bits.OCM = PWM_OFF;
    }
    if(ucChannel == 3)
    {
        //OC3CON2bits.OCTRIS = 1;
        OC3CON1bits.OCM = PWM_OFF;
    }
    if(ucChannel == 4)
    {
        //OC4CON2bits.OCTRIS = 1;
        OC4CON1bits.OCM = PWM_OFF;
    }
    if(ucChannel == 5)
    {
        //OC5CON2bits.OCTRIS = 1;
        OC5CON1bits.OCM = PWM_OFF;
    }
    if(ucChannel == 6)
    {
        //OC6CON2bits.OCTRIS = 1;
        OC6CON1bits.OCM = PWM_OFF;
    }
    if(ucChannel == 7)
    {
        //OC7CON2bits.OCTRIS = 1;
        OC7CON1bits.OCM = PWM_OFF;
    }
    if(ucChannel == 8)
    {
        //OC8CON2bits.OCTRIS = 1;
        OC8CON1bits.OCM = PWM_OFF;
    }

}

