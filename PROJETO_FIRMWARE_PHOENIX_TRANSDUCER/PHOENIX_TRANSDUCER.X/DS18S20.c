#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "timer.h"
//
DS18S20Struct sDS18S20;
unsigned char OneWireCrcCalc(unsigned char *data, unsigned char count);
//
#define DS18S20_BUS_LOW     ONE_WIRE_BUS_PIN = _ENABLE
#define DS18S20_BUS_3STATE  ONE_WIRE_BUS_PIN = _DISABLE
#define DS18S20_BUS_INPUT   ONE_WIRE_BUS_TRIS = INPUT
#define DS18S20_BUS_OUTPUT  ONE_WIRE_BUS_TRIS = OUTPUT
//
void DS18S20Delay(unsigned int ucDelay)
{
    unsigned int i;
    for(i=0;i<ucDelay;i++)
        ClrWdt();
}
//
void DS18S20Init()
{
    DS18S20_BUS_OUTPUT;         //1-Wire bus como TX
    DS18S20_BUS_LOW;            //Pulso de Reset
    DS18S20Delay(7000);
    DS18S20_BUS_INPUT;          //1-Wire bus como RX
    while(ONE_WIRE_BUS_PIN);    //Aguarda pelo pulso de Presenï¿½a
    Nop();
    Nop();
    DS18S20Delay(5000);
}
//
void DS18S20WriteBit(unsigned int bData)
{
    DS18S20_BUS_OUTPUT;         //1-Wire bus como TX
    DS18S20_BUS_LOW;            //Barramento em 0
    DS18S20Delay(160);          //18us
    if(bData == 1)
        DS18S20_BUS_INPUT;
    DS18S20Delay(175);          //25us
    DS18S20_BUS_INPUT;
    DS18S20Delay(20);           //2.2us
}
//
unsigned int DS18S20ReadBit(void)
{
    unsigned int bData;
    DS18S20_BUS_OUTPUT;         //1-Wire bus como TX
    DS18S20_BUS_LOW;            //Barramento em 0
    DS18S20Delay(10);           //2.2us
    DS18S20_BUS_INPUT;
    DS18S20Delay(10);          //11us
    bData = ONE_WIRE_BUS_PIN;
    DS18S20Delay(400);          //45us
    return(bData);
}
//
void DS18S20WriteByte(unsigned char ucData)
{
    unsigned char i,temp;
    for(i=0;i<8;i++)
    {
        temp = ucData >> i;
        temp &= 0x01;
        DS18S20WriteBit(temp);
    }
    DS18S20Delay(10);

}
//
unsigned char DS18S20ReadByte(void)
{
    unsigned char i, value;
    value = 0;
    for(i=0;i<8;i++)
    {
        if(DS18S20ReadBit())
            value |= (1 << i);
    }
    return(value);
}
//
//
void DS18S20StartConversion(void)
{
    //Inicia a conversï¿½o da temperatura
    //O tempo total para terminar a conversï¿½o ï¿½ de 750ms
    //Ler o conteï¿½do da memï¿½ria apï¿½s esse tempo
    DS18S20Init();
    DS18S20WriteByte(0xCC);
    DS18S20WriteByte(0x44);
    //while(!DS18S20ReadBit());
}
//
unsigned char DS18S20GetRAM(void)
{
    unsigned char aData[10];
    unsigned char i;
    int iTempVar;
    //DISABLE_ALL_INTERRUPTS;
    //IEC1bits.INT1IE = 0;
    DS18S20Init();
    DS18S20WriteByte(0xCC);
    DS18S20WriteByte(0xBE);
    for(i=0;i<=8;i++)
        aData[i] = DS18S20ReadByte();
    //ENABLE_ALL_INTERRUPTS;
    //IEC1bits.INT1IE = 1;
    if(OneWireCrcCalc(aData,9) == 0x00)
    {
        //O CRC é válido
        sDS18S20.Temperature = 0;
        sDS18S20.Temperature = aData[1] <<8;
        sDS18S20.Temperature |= aData[0];
        sDS18S20.UserByte1 = aData[2];
        sDS18S20.UserByte2 = aData[3];
        sDS18S20.CountRemain = aData[6];
        sDS18S20.CountPerC = aData[7];
        sDS18S20.Crc= aData[8];
        iTempVar = (sDS18S20.Temperature * 50)-25;  //Aplica escala x100 (/2 x100 pois cada bit equivale a 0.5ï¿½C)
        sDS18S20.Temperature = iTempVar + ((sDS18S20.CountPerC *100)-(sDS18S20.CountRemain *100)) / (sDS18S20.CountPerC *100);
        return 0x00;
    }
    else
    {
        //O CRC é inválido
        return 0xFF;
    }
}
//
unsigned char DS18S20GetROM(void)
{
    unsigned char aData[10];
    unsigned char i;
    DS18S20Init();
    DS18S20WriteByte(0x33);
    for(i=0;i<8;i++)
        aData[i] = DS18S20ReadByte();
    if(OneWireCrcCalc(aData,8) == 0x00)
    {
        //CRC Vï¿½lido
        sDS18S20.FamilyCode = aData[0];
        for(i=0; i<6; i++)
        {
            sDS18S20.SerialNumber[i] = aData[i+1];
        }
        return 0x00;
    }
    else
    {
        //CRC Invï¿½lido
        //O CRC ï¿½ invï¿½lido
        return 0xFF;
    }
}
const unsigned char crc88540_table[256] = {
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
  157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
   35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
  190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
   70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
  219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
  101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
  248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
  140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
   17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
  175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
   50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
  202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
   87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
  233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
  116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};

unsigned char OneWireCrcCalc(unsigned char *data, unsigned char count)
{
    unsigned char result=0;

    while(count--) {
      result = crc88540_table[result ^ *data++];
    }

    return result;
}
