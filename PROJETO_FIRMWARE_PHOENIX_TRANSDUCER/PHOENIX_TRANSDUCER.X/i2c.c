//
// Título:  ITIB1 - I2C Handler
// Versão:  1.0.0
// Criação: 27/Março/2013
// Lançamento:
//
//                             DESCRIÇÃO:
// Este arquivo contém o código para manipulação dos canais I2C da placa ITIB-1.
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
//  | 1.0.0    | 27/03/2013 | Reginaldo do Prado   | Criação do arquivo          |
//  +----------+------------+----------------------+-----------------------------+
//
//
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "timer.h"
//
//
#define FRAM_WRITE_DEVICE_ADDR  0xA0
#define FRAM_READ_DEVICE_ADDR   0xA1
//
//
void I2c1Init(void)
{
    I2C1CONbits.I2CEN = 1;                      //Habilita módulo I2C
    I2C1CONbits.I2CSIDL = 1;                    //Discontinue module operation when device enters Idle mode
    I2C1CONbits.IPMIEN = 0;                     //IPMI Support mode disabled
    I2C1CONbits.A10M = 0;                       //I2CxADD register is a 7-bit slave address
    I2C1CONbits.DISSLW = 0;                     //Slew rate control enabled
    I2C1CONbits.SMEN = 0;                       //Disable SMBus input thresholds
    I2C1CONbits.ACKDT = 1;                      //Send ACK during Acknowledge
    I2C1CONbits.RCEN = 0;                       //Enables Receive mode for I2C
    I2C1BRG = I2C_BAUDRATE;                     //Configura o baud rate
    
    IEC1bits.MI2C1IE = 0;                       //Habilita interrupções do módulo I2C
}
//
//
void I2C2Init(void)
{
    
    
}
//
//
void I2c1InterruptHandler(void)
{

    IFS1bits.MI2C1IF = 0;                       //Zera flag de interrupção

}
//
//
void I2c2InterruptHandler(void)
{

    IFS3bits.MI2C2IF = 0;                       //Zera flag de interrupção

}

void I2c1WriteByte(unsigned int uiAddress, unsigned char ucData)
{
    I2C1CONbits.SEN = 1;                    //Envia Start
    while(I2C1CONbits.SEN);
    I2C1TRN = FRAM_WRITE_DEVICE_ADDR;       //Envia Address Slave
    while(I2C1STATbits.TRSTAT);
    I2C1TRN = (uiAddress>>8);               //Envia Address Hi Byte
    while(I2C1STATbits.TRSTAT);
    I2C1TRN = uiAddress & 0x00FF;           //Envia Address Low Byte
    while(I2C1STATbits.TRSTAT);
    I2C1TRN = ucData;                       //Envia byte de dados
    while(I2C1STATbits.TRSTAT);
    I2C1CONbits.PEN = 1;                    //Envia Stop
    while(I2C1CONbits.PEN);
//    #if(NVRAM_IC_TYPE == 1)
//    //Delay para gravação na EEPROM
//    TimerLoad(TIMEOUT_TIMER,10,RUN);
//    while(TimerStatus(TIMEOUT_TIMER)==RUN);
//    #endif
}
//
//
unsigned char I2c1ReadByte(unsigned int uiAddress)
{
    I2C1CONbits.SEN = 1;                    //Envia Start
    while(I2C1CONbits.SEN);
    I2C1TRN = FRAM_WRITE_DEVICE_ADDR;       //Envia Address Slave
    while(I2C1STATbits.TRSTAT);
    I2C1TRN = (uiAddress>>8);               //Envia Address Hi Byte
    while(I2C1STATbits.TRSTAT);
    I2C1TRN = uiAddress & 0x00FF;           //Envia Address Low Byte
    while(I2C1STATbits.TRSTAT);
    I2C1CONbits.SEN = 1;                    //Envia Re-Start
    while(I2C1CONbits.SEN);
    I2C1TRN = FRAM_READ_DEVICE_ADDR;        //Envia Address Slave
    while(I2C1STATbits.TRSTAT);
    I2C1CONbits.RCEN = 1;                   //Habilita recepção I2C
    while(I2C1CONbits.RCEN);                //Aguarda recepção do byte solicitado
    I2C1CONbits.PEN = 1;                    //Envia Stop
    while(I2C1CONbits.PEN);
    return I2C1RCV;
}
//
void I2c1WriteWord(unsigned int uiAddress, unsigned int uiData)
{
    unsigned char ucTempData;
    ucTempData = (char)uiData;
    I2c1WriteByte(uiAddress,ucTempData);
    uiData = uiData>>8;
    ucTempData = (char)uiData;
    I2c1WriteByte(uiAddress+1,ucTempData);
}
//
unsigned int I2c1ReadWord(unsigned int uiAddress)
{
    unsigned int uiTempData;
    uiTempData = I2c1ReadByte(uiAddress+1);
    uiTempData = (uiTempData<<8) & 0xFF00;
    Nop();
    Nop();
    uiTempData |= I2c1ReadByte(uiAddress);
    Nop();
    Nop();
    return uiTempData;
}
//
//
void I2c1WriteLong(unsigned int uiAddress, long ulData)
{
    unsigned char ucTempData;
    //
    ucTempData = (char)(ulData >> 24);
    I2c1WriteByte(uiAddress,ucTempData);
    //
    ucTempData = (char)(ulData >> 16);
    I2c1WriteByte(uiAddress+1,ucTempData);    
    //
    ucTempData = (char)(ulData >> 8);
    I2c1WriteByte(uiAddress+2,ucTempData); 
    //
    ucTempData = (char)ulData;
    I2c1WriteByte(uiAddress+3,ucTempData);
}
//
long I2c1ReadLong(unsigned int uiAddress)
{
    long lTempData, lTempData2;
    //
    lTempData = (long)I2c1ReadByte(uiAddress);
    lTempData2 = (lTempData<<24) & 0xFF000000;
    //
    lTempData = (long)I2c1ReadByte(uiAddress+1);
    lTempData2 |= (lTempData<<16) & 0xFFFF0000;    
    //
    lTempData = (long)I2c1ReadByte(uiAddress+2);
    lTempData2 |= (lTempData<<8) & 0xFFFFFF00; 
    //
    lTempData = (long)I2c1ReadByte(uiAddress+3);
    lTempData2 |= lTempData;    
    return lTempData2;
}