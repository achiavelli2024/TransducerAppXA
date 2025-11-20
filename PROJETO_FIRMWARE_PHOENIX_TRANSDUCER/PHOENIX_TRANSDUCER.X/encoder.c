//
// Título:  PHOENIX TRANSDUCER - ENCODER Handler
// Versão:  1.0.0
// Criação: 29/Julho/2016
// Lançamento:
//
//                             DESCRIÇÃO:
// Este arquivo contém o código para manipulação do encoder.
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
//  | 1.0.0    | 27/07/2016 | Reginaldo do Prado   | Criação do arquivo          |
//  +----------+------------+----------------------+-----------------------------+
//
//
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "spi.h"
#include "timer.h"
//
void GetEncoderPinState(void);
//
EncoderStruct sEncoder;
//
void EncoderInit()
{
    sEncoder.PulseAcc = 0;
    GetEncoderPinState();
    sEncoder.PrevPinState = sEncoder.PinState;
    INTCON2bits.INT2EP = 0;
    INTCON2bits.INT3EP = 0;
    IEC3bits.INT3IE = 1;
    IEC1bits.INT2IE = 1;
    
}
void ENC_A_InterruptHandler()
{
    GetEncoderPinState();
    //ClrWdt();
    if(sEncoder.Direction == ENCODER_CW)
        sEncoder.PulseAcc++;
    else
        sEncoder.PulseAcc--;
    INTCON2bits.INT2EP = ~INTCON2bits.INT2EP;
    IFS1bits.INT2IF = 0;
}
//
void ENC_B_InterruptHandler()
{
    GetEncoderPinState();
    //ClrWdt();
    if(sEncoder.Direction == ENCODER_CW)
        sEncoder.PulseAcc++;
    else
        sEncoder.PulseAcc--;    
    INTCON2bits.INT3EP = ~INTCON2bits.INT3EP;
    IFS3bits.INT3IF= 0; 
}
//
void GetEncoderPinState(void)
{
    sEncoder.PinState = ENCODER_A_PIN << 1;
    sEncoder.PinState |= ENCODER_B_PIN;
    switch(sEncoder.PinState)
    {
        case 0b00:
        {
            if(sEncoder.PrevPinState == 0b01)
                sEncoder.Direction = ENCODER_CW;
            else
                if(sEncoder.PrevPinState == 0b10)
                    sEncoder.Direction = ENCODER_CCW;
            break;
        }
        //
        case 0b01:
        {
            if(sEncoder.PrevPinState == 0b11)
                sEncoder.Direction = ENCODER_CW;
            else
                if(sEncoder.PrevPinState == 0b00)
                    sEncoder.Direction = ENCODER_CCW;
            break;
        }        
        //
        case 0b11:
        {
            if(sEncoder.PrevPinState == 0b10)
                sEncoder.Direction = ENCODER_CW;
            else
                if(sEncoder.PrevPinState == 0b01)
                    sEncoder.Direction = ENCODER_CCW;
            break;
        }        
        //
        case 0b10:
        {
            if(sEncoder.PrevPinState == 0b00)
                sEncoder.Direction = ENCODER_CW;
            else
                if(sEncoder.PrevPinState == 0b11)
                    sEncoder.Direction = ENCODER_CCW;
            break;
        }         
    }
    sEncoder.PrevPinState = sEncoder.PinState;
}
