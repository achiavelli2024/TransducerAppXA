//
// Título:  PHOENIX TRANSDUCER - AD Handler
// Versão:  1.0.0
// Criação: 29/Julho/2016
// Lançamento:
//
//                             DESCRIÇÃO:
// Este arquivo contém o código para manipulação dos canais analógicos da placa 1940199900.
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
#include "tool.h"
//
//
extern ToolData sToolParameters;
extern long uiAdSin, uiAdCos;
//
void Ad1Single12BitsSample(unsigned char ucChannel);
//
AnalogChannelStruct sAdChannel;
//
//
const unsigned char romAdFastTable[5] = {SIN_AD_CHANNEL,COS_AD_CHANNEL,TORQUE_AD_CHANNEL,TEMP_AD_CHANNEL};
//
//
void AdInit(void)
{
    AD1CON1bits.ADSIDL = 0;
    AD1CON1bits.ADDMABM = 0;
    AD1CON1bits.AD12B = 1;
    AD1CON1bits.FORM = 0b00;
    AD1CON1bits.SSRC = 0b000;
    AD1CON1bits.SSRCG = 0;
    AD1CON1bits.SIMSAM = 0;
    AD1CON1bits.ASAM = 1;
    AD1CON1bits.SAMP = 0;
    AD1CON1bits.DONE = 0;
    
    //AD1CON2 = 0x0000;
    AD1CON2bits.VCFG = 0b001;
    AD1CON2bits.CSCNA = 0;
    AD1CON2bits.CHPS = 0b00;
    AD1CON2bits.BUFS = 0;
    AD1CON2bits.SMPI = 0b00000;
    AD1CON2bits.BUFM = 0;
    AD1CON2bits.ALTS = 0;

    //AD1CON3 = 0x0003;
    AD1CON3bits.ADCS = 8;
    AD1CON4 = 0x0000;

    AD1CHS123bits.CH123NA = 0b00;
    AD1CHS123bits.CH123SA = 0;
    
    AD1CSSH = 0x0000;
    AD1CSSL = 0x0000;
    AD1CON1bits.ADON = 1;
    IEC0bits.AD1IE = 0;                     //desabilita interrupção do AD 1
    sAdChannel.Sample = 0;                  //Aponta para a primeira amostragem
    sAdChannel.Mode = AD_FAST_MODE;         //Determina modo de aquisição
    sAdChannel.Status = AD_IDLE;
    AD1CHS0 = 0;
}

void Ad1MultipleSample(void)
{
    AD1CON2bits.CHPS = 0b10;
    AD1CHS123bits.CH123NA = 0b00;
    AD1CHS123bits.CH123SA = 0;
    AD1CON1bits.AD12B = 0;
    AD1CHS0 = 0x000D;
    AD1CON1bits.SAMP = 0;       //Inicializa conversão
}

void Ad1Single12BitsSample(unsigned char ucChannel)
{
    AD1CHS0 = ucChannel;
    AD1CON2bits.CHPS = 0b00;
    AD1CON1bits.AD12B = 1;
    AD1CON1bits.SAMP = 0;       //Inicializa conversão
   
}
//void Ad1InterruptHandler(void)
void __attribute__((__interrupt__,no_auto_psv)) _AD1Interrupt(void)
{
    if(sAdChannel.Mode == AD_FAST_MODE)
    {
        if(sAdChannel.Sample == 0)
        {
            sAdChannel.Value[sAdChannel.Sample] = ADC1BUF0;
            sAdChannel.Sample = 1;
            Ad1Single12BitsSample(romAdFastTable[sAdChannel.Sample]);
        }
        else
        {
            if(sAdChannel.Sample == 1)
            {
                sAdChannel.Value[sAdChannel.Sample] = ADC1BUF0;
                sAdChannel.Sample = 2;
                Ad1Single12BitsSample(romAdFastTable[sAdChannel.Sample]);
            }
            else
            {
                if(sAdChannel.Sample == 2)
                {
                    sAdChannel.Value[sAdChannel.Sample] = ADC1BUF0;
                    sAdChannel.Sample = 3;
                    Ad1Single12BitsSample(romAdFastTable[sAdChannel.Sample]);
                }
                else
                {
                    sAdChannel.Value[sAdChannel.Sample] = ADC1BUF0;
                    sAdChannel.Sample = 0;
                    sAdChannel.Status = AD_SAMPLE_COMPLETED;

                }
            }
        }
        
    }
    IFS0bits.AD1IF = 0;
}

void Ad2InterruptHandler(void)
{

    IFS1bits.AD2IF = 0;
}

void Ad1FastSampleStart(void)
{
    sAdChannel.Sample = 0;
    sAdChannel.Mode = AD_FAST_MODE;         //Determina modo de aquisição
    Ad1Single12BitsSample(romAdFastTable[sAdChannel.Sample]);   //Dispara a primeira aquisição
}
//
//
unsigned int GetAnalog(unsigned char ucChannel)
{
   unsigned char ucDelay;
    AD1CHS0 = ucChannel;
    ucDelay = AD_DELAY;
    while(ucDelay > 0)
        ucDelay--;
    AD1CON1bits.SAMP = 0;       //Inicializa conversão
    while(!AD1CON1bits.DONE);
    AD1CON1bits.DONE = 0;
    return ADC1BUF0;


}