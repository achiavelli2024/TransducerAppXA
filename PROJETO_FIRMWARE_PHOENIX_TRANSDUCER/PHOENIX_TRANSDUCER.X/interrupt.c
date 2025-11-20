//
// Título:  ITIB1 - Interrupt Handler
// Versão:  1.0.0
// Criação: 21/Março/2013
// Lançamento:
//
//                             DESCRIÇÃO:
// Este arquivo contém o código para manipulação das interrupções da placa ITIB-1.
// Escrito para o PIC24EP512GP806 e compilador XC16.
//
// Copyright (C) 2013 M.Shimizu Elétrica e Pneumática Ltda. Todos os direitos reservados.
//
//                            AVISO LEGAL:
//
// Este arquivo e todo seu conteúdo é propriedade da M.Shimizu Elétrica e Pneumática Ltda.
// A publicação, distribuição ou modificações, totais or parciais, são expressamente proibidas
// sem autorição da M.Shimizu ou dos seus representantes legais.
//
// Para maiores informações viste www.mshimizu.com.br
//
//  +----------+------------+----------------------+-----------------------------+
//  | Versão   | Data       | Autor                | Comentário                  |
//  +----------+------------+----------------------+-----------------------------+
//  | 1.0.0    | 21/03/2013 | Reginaldo do Prado   | Criação do arquivo          |
//  +----------+------------+----------------------+-----------------------------+
//
//
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "timer.h"
#include "ad.h"
#include "uart.h"
#include "i2c.h"
#include "ads1271.h"
#include "encoder.h"
//
//
void InterruptInit(void)
{

    
}

//void __attribute__((__interrupt__,no_auto_psv)) _T1Interrupt(void)
//{
//    //Interrupção gerada pelo Timer 1
//    //Desvia para a rotina de tratamento da interrupção
//    Timer1InterruptHandler();
//}

void __attribute__((__interrupt__,no_auto_psv)) _INT1Interrupt(void)
{
    //Interrupção gerada pela INT1
    ADS1271InterruptHandler();
    
}

void __attribute__((__interrupt__,no_auto_psv)) _INT2Interrupt(void)
{
    //Interrupção gerada pela INT2
    ENC_A_InterruptHandler();
}

void __attribute__((__interrupt__,no_auto_psv)) _INT3Interrupt(void)
{
    //Interrupção gerada pela INT3
    ENC_B_InterruptHandler();
}

void __attribute__((__interrupt__,no_auto_psv)) _T2Interrupt(void)
{
    //Interrupção gerada pelo Timer 2
    IFS0bits.T2IF = 0;
}

void __attribute__((__interrupt__,no_auto_psv)) _T3Interrupt(void)
{
    //Interrupção gerada pelo Timer 3
    IFS0bits.T3IF = 0;
}

void __attribute__((__interrupt__,no_auto_psv)) _T4Interrupt(void)
{
    //Interrupção gerada pelo Timer 4
    IFS1bits.T4IF = 0;
}
//
void __attribute__((__interrupt__,no_auto_psv)) _AD2Interrupt(void)
{
    //Interrupção gerada pelo módulo AD 2
    Ad2InterruptHandler();
    
}
//
void __attribute__((__interrupt__,no_auto_psv)) _MI2C1Interrupt(void)
{
    //Interrupção gerada pelo canal 1 I2C Master Mode
    I2c1InterruptHandler();

}
//
void __attribute__((__interrupt__,no_auto_psv)) _MI2C2Interrupt(void)
{
    //Interrupção gerada pelo canal 2 I2C Master Mode
    I2c2InterruptHandler();

}
//
void __attribute__((__interrupt__,no_auto_psv)) _StackError(void)
{
    Nop();
    Nop();
}
//
void __attribute__((__interrupt__,no_auto_psv)) _MathError(void)
{
    Nop();
    Nop();
}
//
void __attribute__((__interrupt__,no_auto_psv)) _AddressError(void)
{
    Nop();
    Nop();
    INTCON1 = 0x00;
}
//
void __attribute__((__interrupt__,no_auto_psv)) _OscillatorFail(void)
{
    Nop();
    Nop();
}
//
//void __attribute__((__interrupt__,no_auto_psv)) _OscillatorFail(void)
//{
//    Nop();
//    Nop();
//}
//