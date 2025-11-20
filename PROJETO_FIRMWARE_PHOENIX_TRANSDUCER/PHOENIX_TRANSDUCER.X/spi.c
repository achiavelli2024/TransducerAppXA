//
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"

//SpiBufferStruct sSpiTxBuffer;
//SpiBufferStruct sSpiRxBuffer;

void SpiInit(unsigned char ucChannel)
{

    if(ucChannel == 1)
    {
        //Configura a SPI 1
//        SPI1CON1bits.DISSCK = 0;        //SCK habilitado
//        SPI1CON1bits.DISSDO = 0;        //SDO habilitado
//        SPI1CON1bits.MODE16 = 0;        //8 bits
//        SPI1CON1bits.SMP = 1;           //Input data is sampled at the end of data output time
//        SPI1CON1bits.CKE = 1;           //Serial output data changes on transition from active clock state to Idle clock state
//        SPI1CON1bits.SSEN = 1;          //SSx pin is used for Slave mode
//        SPI1CON1bits.CKP = 0;           //Idle state for clock is a low level; active state is a high level
//        SPI1CON1bits.MSTEN = 1;         //Master Mode
//        SPI1CON1bits.SPRE = 0b011;      // Fcy/40
//        SPI1CON1bits.PPRE = 0b01;       // div por 16 (CLK = 500kHz)
        SPI1CON1bits.DISSCK = 0;        //SCK habilitado
        SPI1CON1bits.DISSDO = 0;        //SDO habilitado
        SPI1CON1bits.MODE16 = 0;        //8 bits
        SPI1CON1bits.MSTEN = 1;         //Master Mode
        SPI1CON1bits.SMP = 0;           //Input data is sampled at the end of data output time
        SPI1CON1bits.CKE = 1;           //Serial output data changes on transition from active clock state to Idle clock state
        SPI1CON1bits.SSEN = 0;          //SSx pin is used for Slave mode
        SPI1CON1bits.CKP = 0;           //Idle state for clock is a low level; active state is a high level
        SPI1CON1bits.SPRE = 0b111;      // 0b110;
        SPI1CON1bits.PPRE = 0b01;       // 0b01;
        SPI1STATbits.SPIEN = 1;         //Habilita m�dulo SPI        
    }
//
    if(ucChannel == 2)
    {

        //Configura a SPI 2
        SPI2CON1bits.DISSCK = 0;        //SCK habilitado
        SPI2CON1bits.DISSDO = 0;        //SDO habilitado
        SPI2CON1bits.MODE16 = 0;        //8 bits
        SPI2CON1bits.MSTEN = 1;         //Master Mode
        SPI2CON1bits.SMP = 0;           //Input data is sampled at the end of data output time
        SPI2CON1bits.CKE = 1;           //Serial output data changes on transition from active clock state to Idle clock state
        SPI2CON1bits.SSEN = 0;          //SSx pin is used for Slave mode
        SPI2CON1bits.CKP = 0;           //Idle state for clock is a low level; active state is a high level
//        SPI2CON1bits.SPRE = 0b011;      // Fcy/40
//        SPI2CON1bits.PPRE = 0b11;       // div por 16 (CLK = 500kHz)
        SPI2CON1bits.SPRE = 0b011;      // Fcy/40
        SPI2CON1bits.PPRE = 0b01;       // div por 16 (CLK = 500kHz)
        SPI2STATbits.SPIEN = 1;         //Habilita m�dulo SPI
    }

}
//

