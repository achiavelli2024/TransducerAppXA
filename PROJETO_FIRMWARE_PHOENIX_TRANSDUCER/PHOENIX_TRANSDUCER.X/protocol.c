#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "uart.h"
#include "tool.h"
#include "i2c.h"
#include "ad.h"
#include "pwm.h"
#include "timer.h"
#include "ads1271.h"
#include "encoder.h"
#include "ds18s20.h"
#include "butterworth.h"

long lADS1271Data1[OFFSET_SAMPLES];

unsigned char ucIdReceived;
unsigned char ucTempBuffer[650];//150
unsigned char ucTempBuffer2[8];
long lTemp1, lTemp2;
long ulTempBuffer[5];
unsigned char ucBufferSize = 0;
extern unsigned char ucCrcResult;
extern unsigned char *ucPtr;
extern unsigned char ucTempVar;
extern const unsigned char romId[ID_SIZE+1];
extern const unsigned char romSerialNumber[SERIAL_SIZE+1];
extern const unsigned char romHwVersion[VERSION_SIZE+1];
extern const unsigned char romFwVersion[VERSION_SIZE+1];
extern const unsigned char romModel[MODEL_SIZE+1];
extern unsigned int uiTempVar;
extern long long l64Temp;
unsigned int i;
extern unsigned char ucUartSel;
extern long lTorqueOffset;
extern unsigned char ucTransparentRxCounter;
extern unsigned char ucTransparentEscape;
extern ButterStruct sButter;
//
extern unsigned char ucIdAscii[12];
extern unsigned char ucIdAsciiAddress[12];
unsigned char ucTempAscii[12];
unsigned char ucTempAscii2[12];
extern DeviceStruct sDevice, sTempDevice;
SendLogStruct sSendLog;
//
unsigned char BufferCompare(unsigned char *ucBuf1,unsigned char *ucBuf2,unsigned char ucSize);
unsigned int CheckSum(unsigned int *ptrBuffer, unsigned char ucSize);
int AsciiHex2Int(unsigned char *ptrAscii);
long AsciiHex2Long(unsigned char *ptrAscii);
void Registry2Buffer(unsigned int RegistryAddress, unsigned char *ptrBuffer);
void AnalogSample(void);
void StringCapacity2Num(void);
void Registry2BufferAscii(unsigned int RegistryAddress, unsigned char *ptrBuffer);
//
//
void CrcBufferCalc(unsigned char *ptrBuffer, unsigned int ucSize)
{
    unsigned int ucCounter;
    ucPtr=(unsigned char *)&CRCDATL;            //Aponta para o primeiro byte da FIFO do CRC
    CRCWDATL = 0x0000;                          //Zera resultado de CRC (para n�o influenciar na conta)
    CRCWDATH = 0x0000;                          //Zera resultado de CRC (para n�o influenciar na conta)
    for(ucCounter = 0; ucCounter < ucSize; ucCounter++)
    {
        if(CRCCON1bits.CRCFUL == 0)
        {
            *ucPtr = (unsigned char) ptrBuffer[ucCounter];//Move dado do buffer para a FIFO do CRC
        }
        else
        {
            //FIFO cheia
            CRCCON1bits.CRCGO = 1;              // Dispara hardware de c�lculo do CRC
            while(CRCCON1bits.CRCFUL);          //Espera liberar espa�o na FIFO
            *ucPtr = (unsigned char) ptrBuffer[ucCounter];//Move dado do buffer para a FIFO do CRC
        }
    }
    if(CRCCON1bits.CRCFUL == 1)
    {
        //FIFO cheia
        CRCCON1bits.CRCGO = 1;                  // Dispara hardware de c�lculo do CRC
        while(CRCCON1bits.CRCFUL);              //Espera liberar espa�o na FIFO
    }
    *ucPtr = 0x00;                              //Move "zero" como �ltimo byte da FIFO(Ningu�m sabe pq!!!)
    CRCCON1bits.CRCGO = 1;                      // Dispara hardware de c�lculo do CRC
    while(IFS4bits.CRCIF!=1);                   // Aguarda hardware finalizar o c�lculo de CRC
    IFS4bits.CRCIF = 0;                         // Zera flag de interrup��o do CRC
    CRCCON1bits.CRCGO = 0;                      // Para hardware de CRC
    ucCrcResult = (unsigned char)CRCWDATL;      //Salva CRC computado no buffer
}
//
void ByteHex2Ascii(unsigned char ucByte, unsigned char *ptrBuffer)
{
    //Converte ucByte em dois caracteres ASCII
    //O resultado ser� armazenado nos endere�os 0 e 1 apontados por ptrBuffer
    unsigned char ucTemp;
    ucTemp = ucByte;
    ucTemp = (ucTemp >> 4) & 0b00001111;    //Isola nibble superior
    if(ucTemp <= 9)
        ucTemp = ucTemp + 0x30;             //Se 0...9 converte em 0x30...0x39
    else
        ucTemp = ucTemp + 0x37;             //Se A...F converte em 0x41...0x46
    *ptrBuffer = ucTemp;                    //Armazena resultado
    ptrBuffer++;                            //Aponta para o pr�ximo byte no buffer
    ucTemp = ucByte;
    ucTemp = ucTemp & 0b00001111;           //Isola nibble inferior
    if(ucTemp <= 9)
        ucTemp = ucTemp + 0x30;             //Se 0...9 converte em 0x30...0x39
    else
        ucTemp = ucTemp + 0x37;             //Se A...F converte em 0x41...0x46
    *ptrBuffer = ucTemp;                    //Armazena resultado
}
//
//
unsigned char ByteAscii2Hex(unsigned char *ptrAscii)
{
    //Converte 2 caracteres ASCII apontados por ptrAscci em um byte
    //O resultado � retorno da fun��o
    unsigned char ucTemp, ucTemp2 = 0;
    ucTemp = *ptrAscii;
    if(ucTemp >= '0' && ucTemp <= '9')
    {
        //Aqui est� entre 0x31 e 0x39
        ucTemp2 = ucTemp - 0x30;
    }
    else
    {
        if(ucTemp >= 'A' && ucTemp <= 'F')
        {
            //Aqui est� entre 0x41 e 0x46
            ucTemp2 = ucTemp - 0x37;
        }
    }
    ptrAscii++;                                 //Aponta para o segundo caractere
    ucTemp = *ptrAscii;
    if(ucTemp >= '0' && ucTemp <= '9')
    {
        //Aqui est� entre 0x31 e 0x39
        ucTemp = ucTemp - 0x30;
    }
    else
    {
        if(ucTemp >= 'A' && ucTemp <= 'F')
        {
            //Aqui est� entre 0x41 e 0x46
            ucTemp = ucTemp - 0x37;
        }
    }
    ucTemp2 = (ucTemp2 << 4) & 0b11110000;
    return (ucTemp2 | ucTemp);
}
//
//
void CmdResponse(unsigned char *ucCmd, unsigned int ucCmdSize)//ucCmdSize era uchar
{
    //
    //Envia comando para a placa da ferramenta
    //Esta fun��o envia apenas o primeiro byte da solicita��o
    //O demais bytes s�o gerenciados pela rotina de interrup��o de TX
    //
    if(ucUartSel == COM1)
    {
        if(sCOM1TxBuffer.Status == SLOWCOMM_TX_EMPTY || sCOM1TxBuffer.Status == SLOWCOMM_TX_DONE)
        {
            //unsigned char ucTemp;
            
            #if(PRODUCT_TYPE == 0)
                RTS_EXP_PIN = ON;
            #endif
            sCOM1TxBuffer.Status = SLOWCOMM_TX_EMPTY;       //Zera status do buffer de TX
            sCOM1TxBuffer.Data[0] = STX;                    //Move Prefixo para o buffer
            //Inclui o ID
            #if(PRODUCT_TYPE == 0)
                ByteHex2Ascii(sDevice.ExpAddress,sCOM1TxBuffer.Data+1);
            #else
            ByteHex2Ascii(sDS18S20.SerialNumber[5],sCOM1TxBuffer.Data+1);
            #endif
            ByteHex2Ascii(sDS18S20.SerialNumber[4],sCOM1TxBuffer.Data+3);
            ByteHex2Ascii(sDS18S20.SerialNumber[3],sCOM1TxBuffer.Data+5);
            ByteHex2Ascii(sDS18S20.SerialNumber[2],sCOM1TxBuffer.Data+7);
            ByteHex2Ascii(sDS18S20.SerialNumber[1],sCOM1TxBuffer.Data+9);
            ByteHex2Ascii(sDS18S20.SerialNumber[0],sCOM1TxBuffer.Data+11);
            for(uiTempVar = 0;uiTempVar < ucCmdSize; uiTempVar++)
            {
                sCOM1TxBuffer.Data[uiTempVar+13] = ucCmd[uiTempVar];   //Move primeiro byte do comando
            }
            sCOM1TxBuffer.Data[ucCmdSize+15] = ETX;          //Move sufixo para o buffer
            CrcBufferCalc(sCOM1TxBuffer.Data+1,ucCmdSize+12);  //Calcula o CRC
            ByteHex2Ascii(ucCrcResult,sCOM1TxBuffer.Data+ucCmdSize+13);//Salva CRC em ASCII no buffer
            sCOM1TxBuffer.Pointer = 0;                      //Aponta para o primeiro byte a ser transmitido
            sCOM1TxBuffer.Size = ucCmdSize+16;               //Determina o tamanho do buffer
            COM1TxStart();                               //Dispara a transmiss�o pela UART
            sDevice.CommAct = COMM_LED_BLINK;
            if(sDevice.CommActState ==  STATUS_LED_NORMAL)
            {
                sDevice.CommActState = STATUS_LED_RXTX;
                TimerLoad(LED_TIMER,COMM_LED_RATE,RUN);
            }
        }
    }
    else if(ucUartSel == COM2)
    {
        if(sCOM2TxBuffer.Status == SLOWCOMM_TX_EMPTY || sCOM2TxBuffer.Status == SLOWCOMM_TX_DONE)
        {
            unsigned char ucTemp;

            sCOM2TxBuffer.Status = SLOWCOMM_TX_EMPTY;       //Zera status do buffer de TX
            sCOM2TxBuffer.Data[0] = STX;                    //Move Prefixo para o buffer
            //Inclui o ID
            ByteHex2Ascii(sDS18S20.SerialNumber[5],sCOM2TxBuffer.Data+1);
            ByteHex2Ascii(sDS18S20.SerialNumber[4],sCOM2TxBuffer.Data+3);
            ByteHex2Ascii(sDS18S20.SerialNumber[3],sCOM2TxBuffer.Data+5);
            ByteHex2Ascii(sDS18S20.SerialNumber[2],sCOM2TxBuffer.Data+7);
            ByteHex2Ascii(sDS18S20.SerialNumber[1],sCOM2TxBuffer.Data+9);
            ByteHex2Ascii(sDS18S20.SerialNumber[0],sCOM2TxBuffer.Data+11);
            for(ucTemp = 0;ucTemp < ucCmdSize; ucTemp++)
            {
                sCOM2TxBuffer.Data[ucTemp+13] = ucCmd[ucTemp];   //Move primeiro byte do comando
            }
            sCOM2TxBuffer.Data[ucCmdSize+15] = ETX;          //Move sufixo para o buffer
            CrcBufferCalc(sCOM2TxBuffer.Data+1,ucCmdSize+12);  //Calcula o CRC
            ByteHex2Ascii(ucCrcResult,sCOM2TxBuffer.Data+ucCmdSize+13);//Salva CRC em ASCII no buffer
            sCOM2TxBuffer.Pointer = 0;                      //Aponta para o primeiro byte a ser transmitido
            sCOM2TxBuffer.Size = ucCmdSize+16;               //Determina o tamanho do buffer
            COM2TxStart();                               //Dispara a transmiss�o pela UART
            sDevice.CommAct = COMM_LED_BLINK;
            if(sDevice.CommActState ==  STATUS_LED_NORMAL)
            {
                sDevice.CommActState = STATUS_LED_RXTX;
                TimerLoad(LED_TIMER,COMM_LED_RATE,RUN);
            }
        }            
    }
}
//
void CmdGraphResponse(unsigned int uiCmdSize)
{
    //
    //Envia comando para a placa da ferramenta
    //Esta fun��o envia apenas o primeiro byte da solicita��o
    //O demais bytes s�o gerenciados pela rotina de interrup��o de TX
    //
    if(ucUartSel == COM1)
    {
        if(sCOM1TxBuffer.Status == SLOWCOMM_TX_EMPTY || sCOM1TxBuffer.Status == SLOWCOMM_TX_DONE)
        {
            //unsigned char ucTemp;
            #if(PRODUCT_TYPE == 0)
                RTS_EXP_PIN = ON;
            #endif
            sCOM1TxBuffer.Status = SLOWCOMM_TX_EMPTY;       //Zera status do buffer de TX
            sCOM1TxBuffer.Data[0] = STX;                    //Move Prefixo para o buffer
            //Inclui o ID
                        //Inclui o ID
            #if(PRODUCT_TYPE == 0)
                ByteHex2Ascii(sDevice.ExpAddress,sCOM1TxBuffer.Data+1);
            #else
                ByteHex2Ascii(sDS18S20.SerialNumber[5],sCOM1TxBuffer.Data+1);
            #endif
            ByteHex2Ascii(sDevice.ExpAddress,sCOM1TxBuffer.Data+1);
            ByteHex2Ascii(sDS18S20.SerialNumber[4],sCOM1TxBuffer.Data+3);
            ByteHex2Ascii(sDS18S20.SerialNumber[3],sCOM1TxBuffer.Data+5);
            ByteHex2Ascii(sDS18S20.SerialNumber[2],sCOM1TxBuffer.Data+7);
            ByteHex2Ascii(sDS18S20.SerialNumber[1],sCOM1TxBuffer.Data+9);
            ByteHex2Ascii(sDS18S20.SerialNumber[0],sCOM1TxBuffer.Data+11);
            //
            sCOM1TxBuffer.Data[13] ='G';
            sCOM1TxBuffer.Data[14] ='D';

            sCOM1TxBuffer.Data[uiCmdSize+17] = ETX;          //Move sufixo para o buffer
            CrcBufferCalc(sCOM1TxBuffer.Data+1,uiCmdSize+14);  //Calcula o CRC
            ByteHex2Ascii(ucCrcResult,sCOM1TxBuffer.Data+uiCmdSize+15);//Salva CRC em ASCII no buffer
            sCOM1TxBuffer.Pointer = 0;                      //Aponta para o primeiro byte a ser transmitido
            sCOM1TxBuffer.Size = uiCmdSize+18;               //Determina o tamanho do buffer
            COM1TxStart();                               //Dispara a transmiss�o pela UART
            sDevice.CommAct = COMM_LED_BLINK;
            if(sDevice.CommActState ==  STATUS_LED_NORMAL)
            {
                sDevice.CommActState = STATUS_LED_RXTX;
                TimerLoad(LED_TIMER,COMM_LED_RATE,RUN);
            }
        }
    }
    else if(ucUartSel == COM2)
    {
        if(sCOM2TxBuffer.Status == SLOWCOMM_TX_EMPTY || sCOM2TxBuffer.Status == SLOWCOMM_TX_DONE)
        {
            //unsigned char ucTemp;
            sCOM2TxBuffer.Status = SLOWCOMM_TX_EMPTY;       //Zera status do buffer de TX
            sCOM2TxBuffer.Data[0] = STX;                    //Move Prefixo para o buffer
            //Inclui o ID
            ByteHex2Ascii(sDS18S20.SerialNumber[5],sCOM2TxBuffer.Data+1);
            ByteHex2Ascii(sDS18S20.SerialNumber[4],sCOM2TxBuffer.Data+3);
            ByteHex2Ascii(sDS18S20.SerialNumber[3],sCOM2TxBuffer.Data+5);
            ByteHex2Ascii(sDS18S20.SerialNumber[2],sCOM2TxBuffer.Data+7);
            ByteHex2Ascii(sDS18S20.SerialNumber[1],sCOM2TxBuffer.Data+9);
            ByteHex2Ascii(sDS18S20.SerialNumber[0],sCOM2TxBuffer.Data+11);
            //
            sCOM2TxBuffer.Data[13] ='G';
            sCOM2TxBuffer.Data[14] ='D';
            sCOM2TxBuffer.Data[uiCmdSize+17] = ETX;          //Move sufixo para o buffer
            CrcBufferCalc(sCOM2TxBuffer.Data+1,uiCmdSize+14);  //Calcula o CRC
            ByteHex2Ascii(ucCrcResult,sCOM2TxBuffer.Data+uiCmdSize+15);//Salva CRC em ASCII no buffer
            sCOM2TxBuffer.Pointer = 0;                      //Aponta para o primeiro byte a ser transmitido
            sCOM2TxBuffer.Size = uiCmdSize+18;               //Determina o tamanho do buffer
            COM2TxStart();                               //Dispara a transmissão pela UART
            sDevice.CommAct = COMM_LED_BLINK;
            if(sDevice.CommActState ==  STATUS_LED_NORMAL)
            {
                sDevice.CommActState = STATUS_LED_RXTX;
                TimerLoad(LED_TIMER,COMM_LED_RATE,RUN);
            }
        } 
    }
}
//
void RxCmdValidation(void)
{
    //Foi recebido um pacote completo pela UART
    //Validar CRC
    //Verificar se é a resposta do ID
    //Validar ID
    unsigned char i, ucIndex;
    unsigned int uiCommand, uiTempData;
    long lTemp;
    int uiTemp;
    unsigned char ucTemp2;
    unsigned char ucTemp;
    unsigned char ucCommandStatus;
    
    ucCommandStatus = NO_ERROR;
    if(ucUartSel == COM1)
    {
        for(i=0; i<sCOM1RxBuffer.Size; i++)
        {
            sCOM2RxBuffer.Data[i] = sCOM1RxBuffer.Data[i];
            sCOM2RxBuffer.Size = sCOM1RxBuffer.Size;
        }
    }
    CrcBufferCalc(sCOM2RxBuffer.Data+1,sCOM2RxBuffer.Size-4);          //Calcula o CRC do buffer recebido
    ucTempVar = ByteAscii2Hex(sCOM2RxBuffer.Data + sCOM2RxBuffer.Size-3);
    if(ucCrcResult == ucTempVar)
    {
        //Aqui o CRC é válido
        //Validar o ID
        if((BufferCompare(sCOM2RxBuffer.Data+1, ucIdAscii, 12) == TRUE) || (BufferCompare(sCOM2RxBuffer.Data+1, ucIdAsciiAddress, 12) == TRUE))
        {
            //Reinicia Auto PowerOff Timer
            TimerLoad(AUTO_PWR_OFF_TIMER, sDevice.AutoPowerOff * 1000, RUN);
            sDevice.CommAct = COMM_LED_BLINK;
            if(sDevice.CommActState ==  STATUS_LED_NORMAL)
            {
                sDevice.CommActState = STATUS_LED_RXTX;
                TimerLoad(LED_TIMER,COMM_LED_RATE,RUN);
            }
            //Identificar o comando
            uiCommand = (sCOM2RxBuffer.Data[13] <<8) & 0b1111111100000000;
            uiCommand |= sCOM2RxBuffer.Data[14];
            switch(uiCommand)
            {
                case REQUEST_TORQUE:
                {
                    //Foi recebida solicitação de torque
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        //lADS1271Value -= 0x5dd4;
                        lTemp = lADS1271Value - lTorqueOffset;
                        //
                        ucTempBuffer[0] = 'T';
                        ucTempBuffer[1] = 'Q';
                        ucTempVar = (lTemp >> 24);// & 0x00FF;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2);

                        ucTempVar = (lTemp >> 16);// & 0x00FF;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+4);

                        ucTempVar = (lTemp >> 8);// & 0x00FF;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+6);

                        ucTempVar = (unsigned char)lTemp;// & 0x00FF;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+8);

                        ucTempVar = (unsigned char)(sEncoder.PulseAcc >> 24);// & 0x00FF;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+10);

                        ucTempVar = (unsigned char)(sEncoder.PulseAcc >> 16);// & 0x00FF;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+12);   

                        ucTempVar = (unsigned char)(sEncoder.PulseAcc >> 8);// & 0x00FF;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+14);     

                        ucTempVar = (unsigned char)sEncoder.PulseAcc;// & 0x00FF;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+16);                    

                        CmdResponse(ucTempBuffer, 18);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }   
                //
                case ZERA_OFFSET:
                {
                    //Foi recebida solicitação para zerar offset
                    if(sCOM2RxBuffer.Size == 20)
                    {
                        if(sCOM2RxBuffer.Data[15] == '0' || sCOM2RxBuffer.Data[15] == '1')
                        {
                            if(sCOM2RxBuffer.Data[16] == '0' || sCOM2RxBuffer.Data[16] == '1')
                            {
                                if(sCOM2RxBuffer.Data[15] == '1')
                                {
                                    if(sDevice.ErrorCode != TRANSDUCER_ERROR)
                                    {
                                        TimerLoad(DELAYTIMER,10,RUN);
                                        l64Temp = 0;
                                        for(ucIndex = 0; ucIndex < OFFSET_SAMPLES; ucIndex++)
                                        {
                                            while(TimerStatus(DELAYTIMER)==RUN)
                                                ClrWdt();
                                            l64Temp += lADS1271Value;
                                            TimerLoad(DELAYTIMER,10,RUN);
                                        }
                                        l64Temp = l64Temp/OFFSET_SAMPLES;
                                        lTorqueOffset = (long)l64Temp;
                                        I2c1WriteLong(NVRAM_TORQUE_OFFSET, (unsigned long)lTorqueOffset);
                                        lTorqueOffset = I2c1ReadLong(NVRAM_TORQUE_OFFSET);                                         
                                   }
                               }
                                if(sCOM2RxBuffer.Data[16] == '1')
                                {
                                    //Zera offset do �ngulo
                                    sEncoder.PulseAcc = 0;
                                }
                                ucTempBuffer[0] = 'Z';
                                ucTempBuffer[1] = 'O';
                                //
                                ucTempVar = (lTorqueOffset >> 24);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+2);
                                
                                ucTempVar = (lTorqueOffset >> 16);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+4);
                                
                                ucTempVar = (lTorqueOffset >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+6);
                                
                                ucTempVar = (unsigned char)lTorqueOffset;
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+8);
                                //
                                ucTempVar = (unsigned char)(sEncoder.PulseAcc >> 24);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+10);
                                ucTempVar = (unsigned char)(sEncoder.PulseAcc >> 16);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+12);   
                                ucTempVar = (unsigned char)(sEncoder.PulseAcc >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+14);     
                                ucTempVar = (unsigned char)sEncoder.PulseAcc;
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+16);                                  
                                //
                                CmdResponse(ucTempBuffer, 18);
                            }
                            else
                                ucCommandStatus = SINTAX_ERROR;
                        }
                        else
                            ucCommandStatus = SINTAX_ERROR;
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;

                    break;
                }                
                //
                case READ_ACQ_CONFIG:
                {
                    //Foi recebida solicitação de leitura de configuração de aquisição
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        if(sDevice.AcquisitionReady == ACQ_READY)
                        {
                            ucTempBuffer[0] = 'G';
                            ucTempBuffer[1] = 'A';
                            //
                            //Salva o THI no buffer
                            //
                            ucTempVar = (unsigned char)(sDevice.InitialThreshold >> 24);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+2);
                            ucTempVar = (unsigned char)(sDevice.InitialThreshold >> 16);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+4);
                            ucTempVar = (unsigned char)(sDevice.InitialThreshold >> 8);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+6);
                            ucTempVar = (unsigned char)sDevice.InitialThreshold;
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+8);
                            //
                            //Salva o THI no buffer
                            //
                            ucTempVar = (unsigned char)(sDevice.FinalThreshold >> 24);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+10);
                            ucTempVar = (unsigned char)(sDevice.FinalThreshold >> 16);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+12);
                            ucTempVar = (unsigned char)(sDevice.FinalThreshold >> 8);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+14);
                            ucTempVar = (unsigned char)sDevice.FinalThreshold;
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+16);
                            //
                            //Salva o THT no buffer
                            //
                            ucTempVar = (unsigned char)(sDevice.AcquisitionFinalTimeout >> 8);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+18);
                            ucTempVar = (unsigned char)sDevice.AcquisitionFinalTimeout;
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+20);                        
                            //
                            //Salva o TIME STEP no buffer
                            //
                            ucTempVar = (unsigned char)(sDevice.AcquisitionTimeStep >> 8);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+22);
                            ucTempVar = (unsigned char)sDevice.AcquisitionTimeStep;
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+24); 
                            //
                            //Salva o FREQ no buffer
                            //
                            ucTempVar = (unsigned char)(sDevice.FilterFreq >> 8);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+26);
                            ucTempVar = (unsigned char)sDevice.FilterFreq;
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+28);                            
                            //Salva os bytes reservados no buffer
                            ucTempBuffer[30] = '0';
                            ucTempBuffer[31] = '0';
                            ucTempBuffer[32] = '0';
                            ucTempBuffer[33] = '0';
                            //
                            CmdResponse(ucTempBuffer, 34);
                        }
                        else
                        {
                            //Comando n�o aceito
                            //Aquisi��o n�o liberada/configurada
                            ucCommandStatus = DEVICE_NOT_READY;
                            break;                            
                        }                        
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                } 
                //
                case READ_DEVICE_STATUS:
                {
                    //Foi recebida solicitação de leitura de configuração de aquisição
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        ucTempBuffer[0] = 'D';
                        ucTempBuffer[1] = 'S';
                        //
                        //Salva o STATE no buffer
                        //
                        ucTempVar = (unsigned char)sDevice.State;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2);
                        //
                        //Salva o ERROR CODE no buffer
                        //
                        ucTempVar = (unsigned char)sDevice.ErrorCode;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+4);
                        //
                        //Salva o TEMPERATURA no buffer
                        //
                        ucTempVar = (unsigned char)(sDevice.Temperature >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+6);
                        ucTempVar = (unsigned char)sDevice.Temperature;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+8);                       
                        //
                        //Salva o COMM INTERFACE no buffer
                        //
                        if(ucUartSel == COM1)
                        {
                            sDevice.CommInterface = BLUETOOTH_INTERFACE;
                        }
                        else
                        {
                            if(ucUartSel == COM2)
                            {
                                sDevice.CommInterface = USB_INTERFACE;
                            }
                        }
                        ucTempVar = (unsigned char)sDevice.CommInterface;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+10);
                        //
                        //Salva o POWER SOURCE no buffer
                        //
                        ucTempVar = (unsigned char)sDevice.PowerSource;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+12);
                        //
                        //Salva o POWER STATE no buffer
                        //
                        ucTempVar = (unsigned char)sDevice.PowerState;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+14);
                        //
                        //Salva o ANALOG POWER STATE no buffer
                        //
                        ucTempVar = (unsigned char)sDevice.AnalogPowerState;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+16);   
                        //
                        //Salva o ENCODER POWER STATE no buffer
                        //
                        ucTempVar = (unsigned char)sDevice.EncoderPowerState;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+18);                         
                        //
                        //Salva o POWER VOLTAGE no buffer
                        //
                        ucTempVar = (unsigned char)(sDevice.PowerVoltage >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+20);
                        ucTempVar = (unsigned char)sDevice.PowerVoltage;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+22); 
                        //
                        //Salva o AUTO POWER OFF no buffer
                        //
                        ucTempVar = (unsigned char)(sDevice.AutoPowerOff >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+24);
                        ucTempVar = (unsigned char)sDevice.AutoPowerOff;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+26); 
                        //
                        //Salva o MOTIVO RESET no buffer
                        //
                        ucTempVar = (unsigned char)(sDevice.LastReset >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+28);
                        ucTempVar = (unsigned char)sDevice.LastReset;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+30); 
                        //
                        //Salva o TEMPO DE VIDA no buffer
                        //                        
                        ucTempVar = (unsigned char)(sDevice.Alive >> 24);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+32);
                        ucTempVar = (unsigned char)(sDevice.Alive >> 16);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+34);
                        ucTempVar = (unsigned char)(sDevice.Alive >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+36);
                        ucTempVar = (unsigned char)sDevice.Alive;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+38);
                        //
                        //Salva o OFFSET no buffer
                        ucTempVar = (unsigned char)(lTorqueOffset >> 24);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+40);
                        
                        ucTempVar = (unsigned char)(lTorqueOffset >> 16);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+42);
                        
                        ucTempVar = (unsigned char)(lTorqueOffset >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+44);
                        
                        ucTempVar = (unsigned char)lTorqueOffset;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+46);                         
                        CmdResponse(ucTempBuffer, 48);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }  
                //
                case WRITE_DEVICE_SETUP:
                {
                    //Foi recebida solicitação de gravação de configurações
                    if(sCOM2RxBuffer.Size == 38)
                    {
                        //Obtém AUTO POWER OFF TIMER
                        uiTemp = (unsigned int)AsciiHex2Int(sCOM2RxBuffer.Data+15);
                        if(uiTemp == 0 || (uiTemp >= MIN_AUTO_PWROFF_TIMER && uiTemp <= MAX_AUTO_PWROFF_TIMER))
                        {
                            if(uiTemp == 0)
                            {
                                //desabilita Auto Power OFF timer
                                sDevice.AutoPowerOff = 0;
                                TimerStop(AUTO_PWR_OFF_TIMER);
                                TimerLoad(AUTO_PWR_OFF_TIMER, sDevice.AutoPowerOff, RUN);
                            }
                            else
                            {
                                //Salva valor do Auto Power OFF timer
                                lTemp = (long)uiTemp;
                                //lTemp = lTemp * 1000;   //Converte em ms
                                I2c1WriteLong(NVRAM_AUTO_PWR_OFF_TIMER, (unsigned long)lTemp);
                                sDevice.AutoPowerOff = I2c1ReadLong(NVRAM_AUTO_PWR_OFF_TIMER);
                                TimerLoad(AUTO_PWR_OFF_TIMER, sDevice.AutoPowerOff * 1000, RUN);
                            }
                            ucTempBuffer[0] = 'W';
                            ucTempBuffer[1] = 'S';
                            ucTempBuffer[0] = '0';
                            ucTempBuffer[1] = '0';                        
                            CmdResponse(ucTempBuffer, 4);                            
                        }
                        else
                        {
                            ucCommandStatus = SINTAX_ERROR;
                        }
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }                 
                //
                case READ_ACQ_STATUS:
                {
                    //Foi recebida solicitação de leitura de status de aquisição
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        if(sDevice.AcquisitionReady == ACQ_READY)
                        {
                            ucTempBuffer[0] = 'L';
                            ucTempBuffer[1] = 'S';
                            //
                            //Salva o ACQ STATUS no buffer
                            //
                            ucTempVar = (unsigned char)sDevice.AcquisitionStatus;
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+2);
                            //
                            //Salva o ACQ DIRECTION no buffer
                            //
                            ucTempVar = (unsigned char)sDevice.AcquisitionDirection;
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+4);                        
                            //
                            //Salva o ACQ INDEX no buffer
                            //
                            ucTempVar = (unsigned char)((sDevice.AcquisitionIndex - 1) >> 8);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+6);
                            ucTempVar = (unsigned char)(sDevice.AcquisitionIndex - 1);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+8); 
                            //
                            //Salva o Size no buffer
                            //
                            ucTempVar = (unsigned char)(sDevice.AcquisitionSize >> 8);
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+10);
                            ucTempVar = (unsigned char)sDevice.AcquisitionSize;
                            ByteHex2Ascii(ucTempVar,ucTempBuffer+12);
                            
                            if(sDevice.ToolType == TIPO_TORQUIMETRO_ESTALO)
                            {                            
                                //
                                //Salva o PEAK INDEX no buffer
                                //
                                ucTempVar = (unsigned char)(sDevice.AcquisitionFirstPeakIndex >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+14);
                                ucTempVar = (unsigned char)sDevice.AcquisitionFirstPeakIndex;
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+16); 
                                //
                                //Salva o THT INDEX no buffer
                                //
                                ucTempVar = (unsigned char)(sDevice.AcquisitionTHFIndex >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+18);
                                ucTempVar = (unsigned char)sDevice.AcquisitionTHFIndex;
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+20);  
                                //
                                //Salva o PEAKVALUE no buffer
                                //
                                ucTempVar = (unsigned char)(sDevice.AcquisitionFirstPeak >> 24);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+22);
                                ucTempVar = (unsigned char)(sDevice.AcquisitionFirstPeak >> 16);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+24);                            
                                ucTempVar = (unsigned char)(sDevice.AcquisitionFirstPeak >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+26);
                                ucTempVar = (unsigned char)(sDevice.AcquisitionFirstPeak);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+28);                             
                                //
                                //Salva o PEAKANGLE no buffer
                                //
                                ucTempVar = (unsigned char)(sDevice.AcquisitionPeakAngle >> 24);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+30);
                                ucTempVar = (unsigned char)(sDevice.AcquisitionPeakAngle >> 16);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+32);                            
                                ucTempVar = (unsigned char)(sDevice.AcquisitionPeakAngle >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+34);
                                ucTempVar = (unsigned char)(sDevice.AcquisitionPeakAngle);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+36);  
                            }
                            else
                            {
                                //
                                //Salva o PEAK INDEX no buffer
                                //
                                ucTempVar = (unsigned char)(sDevice.AcquisitionPeakIndex >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+14);
                                ucTempVar = (unsigned char)sDevice.AcquisitionPeakIndex;
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+16); 
                                //
                                //Salva o THT INDEX no buffer
                                //
                                ucTempVar = (unsigned char)(sDevice.AcquisitionTHFIndex >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+18);
                                ucTempVar = (unsigned char)sDevice.AcquisitionTHFIndex;
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+20);  
                                //
                                //Salva o PEAKVALUE no buffer
                                //
                                ucTempVar = (unsigned char)(sDevice.AcquisitionMaxTorque >> 24);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+22);
                                ucTempVar = (unsigned char)(sDevice.AcquisitionMaxTorque >> 16);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+24);                            
                                ucTempVar = (unsigned char)(sDevice.AcquisitionMaxTorque >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+26);
                                ucTempVar = (unsigned char)(sDevice.AcquisitionMaxTorque);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+28);                             
                                //
                                //Salva o PEAKANGLE no buffer
                                //
                                ucTempVar = (unsigned char)(sDevice.AcquisitionPeakAngle >> 24);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+30);
                                ucTempVar = (unsigned char)(sDevice.AcquisitionPeakAngle >> 16);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+32);                            
                                ucTempVar = (unsigned char)(sDevice.AcquisitionPeakAngle >> 8);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+34);
                                ucTempVar = (unsigned char)(sDevice.AcquisitionPeakAngle);
                                ByteHex2Ascii(ucTempVar,ucTempBuffer+36); 
                                
                                
                            }
                            //
                            //Salva os bytes reservados no buffer
                            //
                            ucTempBuffer[38] = '0';
                            ucTempBuffer[39] = '0';
                            ucTempBuffer[40] = '0';
                            ucTempBuffer[41] = '0';
                            ucTempBuffer[42] = '0';
                            ucTempBuffer[43] = '0';
                            ucTempBuffer[44] = '0';
                            ucTempBuffer[45] = '0';
                            //
                            CmdResponse(ucTempBuffer, 46);
                        }
                        else
                        {
                            //Comando n�o aceito
                            //Aquisi��o n�o liberada/configurada
                            ucCommandStatus = DEVICE_NOT_READY;
                            break;                            
                        }
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }    
                //
                case GET_DATA:
                {
                    //Foi recebida solicita��o de leitura dos pontos armazenados
                    if(sCOM2RxBuffer.Size == 26)
                    {
                        if(sDevice.AcquisitionReady == ACQ_READY || sDevice.AcquisitionStatus != ACQ_IDLE)
                        {
                            //Verifica se o endere�o index inicial � v�lido
                            uiTemp = (unsigned int)AsciiHex2Int(sCOM2RxBuffer.Data+15);
                            if(uiTemp < RAM_BUF_SIZE)
                            {
                                sSendLog.StartIndex = uiTemp;
                                //Verifica se o Size est� no intervalo correto
                                ucTemp = ByteAscii2Hex(sCOM2RxBuffer.Data+19);
                                if(ucTemp > 0 && ucTemp <= MAX_GET_DATA_SIZE)
                                {
                                    sSendLog.Size = ucTemp;
                                    //Verifica se o passo est� no intervalo correto
                                    ucTemp = ByteAscii2Hex(sCOM2RxBuffer.Data+21);
                                    if(ucTemp > 0 && ucTemp <= sSendLog.Size)
                                    {
                                        sSendLog.Step = ucTemp - 1;
                                        //Monta o buffer de resposta
                                        if(ucUartSel == COM1)
                                        {
                                            for(i = 0; i < sSendLog.Size; i++)
                                            {
                                                    if(i==0)
                                                        Registry2Buffer(sSendLog.StartIndex + i,sCOM1TxBuffer.Data+15+i*5);
                                                    else
                                                    {
                                                        Registry2Buffer(sSendLog.StartIndex + i + sSendLog.Step * i,sCOM1TxBuffer.Data+15+i*5);
                                                    }
                                            }
                                            CmdGraphResponse(sSendLog.Size * 5);
                                        }
                                        else
                                        {
                                            if(ucUartSel == COM2)
                                            {
                                                for(i = 0; i < sSendLog.Size; i++)
                                                {
                                                        if(i==0)
                                                            Registry2Buffer(sSendLog.StartIndex + i,sCOM2TxBuffer.Data+15+i*5);
                                                        else
                                                        {
                                                            Registry2Buffer(sSendLog.StartIndex + i + sSendLog.Step * i,sCOM2TxBuffer.Data+15+i*5);
                                                        }
                                                }
                                                CmdGraphResponse(sSendLog.Size * 5);                                                
                                            }
                                        }
                                    }
                                    else
                                    {
                                        //Passo incorreto
                                        ucCommandStatus = SINTAX_ERROR;
                                        break;                                
                                    }                                     
                                }
                                else
                                {
                                    //Size incorreto
                                    ucCommandStatus = SINTAX_ERROR;
                                    break;                                
                                }                                
                            }
                            else
                            {
                                //Endere�o inicial inv�lido
                                ucCommandStatus = SINTAX_ERROR;
                                break;                                
                            }

                        }
                        else
                        {
                            //Comando n�o aceito
                            //Aquisi��o n�o liberada/configurada
                            ucCommandStatus = DEVICE_NOT_READY;
                            break;                            
                        }
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }                 
                //
                case READ_DEVICE_INFO:
                {
                    //Foi recebida solicita��o DEVICE INFO
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        ucTempBuffer[0] = 'D';
                        ucTempBuffer[1] = 'I';
                        //
                        //Salva o Serial Number no buffer
                        //
                        for(i = 0; i < SERIAL_SIZE; i++)
                        {
                           ucTempBuffer[i+2] = sDevice.SerialNumber[i]; 
                        }
                        //
                        //Salva o Modelo no buffer
                        //
                        for(i = 0; i < MODEL_SIZE; i++)
                        {
                           ucTempBuffer[i + 2 + SERIAL_SIZE] = sDevice.Model[i];
                        }
                        //
                        //Salva o hardware version no buffer
                        //                        
                        for(i = 0; i < VERSION_SIZE; i++)
                        {
                           ucTempBuffer[i + 2+ SERIAL_SIZE + MODEL_SIZE] = sDevice.HwVersion[i];
                        }
                        //
                        //Salva o firmware version no buffer
                        //                        
                        for(i = 0; i < VERSION_SIZE; i++)
                        {
                           ucTempBuffer[i + 2 +SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE] = sDevice.FwVersion[i];
                        }                        
                        //
                        //Salva o type version no buffer
                        //                        
                        for(i = 0; i < TYPE_SIZE; i++)
                        {
                           ucTempBuffer[i + 2 +SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE] = sDevice.Type[i];
                        }                         
                        //
                        //Salva a capacidade no buffer
                        //                        
                        for(i = 0; i < CAPACITY_SIZE; i++)
                        {
                           ucTempBuffer[i + 2 +SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE] = sDevice.Capacity[i];
                        }
                        //
                        //Salva BUFFER SIZE
                        //                        
                        ucTempVar = (unsigned char)(RAM_BUF_SIZE >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE);
                        ucTempVar = (unsigned char)RAM_BUF_SIZE;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 2);
                        //
                        //Salva TORQUE SENS.
                        //                        
                        ucTempVar = (unsigned char)(sDevice.TorqueSensitivity >> 24);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 4);
                        ucTempVar = (unsigned char)(sDevice.TorqueSensitivity >> 16);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 6); 
                        ucTempVar = (unsigned char)(sDevice.TorqueSensitivity >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 8);                        
                        ucTempVar = (unsigned char)sDevice.TorqueSensitivity;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 10);
                        //
                        //Salva ANGLE SENS.
                        //                        
                        ucTempVar = (unsigned char)(sDevice.AngleSensivity >> 24);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 12);
                        ucTempVar = (unsigned char)(sDevice.AngleSensivity >> 16);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 14); 
                        ucTempVar = (unsigned char)(sDevice.AngleSensivity >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 16);                        
                        ucTempVar = (unsigned char)sDevice.AngleSensivity;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2+SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 18);                        
                        //                        
                        //
                        //Salva BYTES RESERVADOS no buffer
                        //                        
                        for(i = 0; i < 8; i++)
                        {
                           ucTempBuffer[i + 2 +SERIAL_SIZE + MODEL_SIZE + VERSION_SIZE + VERSION_SIZE + TYPE_SIZE + CAPACITY_SIZE + 20] = '0';
                        }                        
                        //
                        CmdResponse(ucTempBuffer, 86);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }  
                //
                case NEW_ACQ:
                {
                    //Foi recebida solicita��o para liberar nova aquisi��o
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        if(sDevice.AcquisitionReady == ACQ_READY)
                        {
                            sDevice.AcquisitionStatus = ACQ_IDLE;
                            sDevice.AcquisitionIndex = 0;
                            sDevice.AcquisitionPeakIndex = 0;
                            sDevice.AcquisitionDirection = CW;
                            sDevice.AcquisitionSize = 0;
                            sDevice.AcquisitionTHFIndex = 0;
                            sDevice.AcquisitionMaxTorque = 0;
                            sDevice.AcquisitionDelayFlag = 0;
                            sDevice.FastScan = 1;
                            ucTempBuffer[0] = 'N';
                            ucTempBuffer[1] = 'A';
                            ucTempBuffer[2] = '0';
                            ucTempBuffer[3] = '0';
                            //
                            CmdResponse(ucTempBuffer, 4);
                        }
                        else
                        {
                            //Comando n�o aceito
                            //Aquisi��o n�o liberada/configurada
                            ucCommandStatus = DEVICE_NOT_READY;
                            break;                            
                        }
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }                 
                //
                case WRITE_ACQ_CONFIG:
                {
                    //Foi recebida solicita��o para gravar as configura��es da aquisi��o
                    if(sCOM2RxBuffer.Size == 50)
                    {
                        //Obt�m o THI
                        lTemp = AsciiHex2Long(sCOM2RxBuffer.Data+15);
                        if(lTemp >= THI_MIN && lTemp <= THI_MAX)
                        {
                            //Est� no intervalo correto
                            sTempDevice.InitialThreshold = lTemp;
                            //
                            //Obt�m o THF
                            lTemp = AsciiHex2Long(sCOM2RxBuffer.Data+23);
                            if(lTemp >= THF_MIN && lTemp <= THF_MAX)
                            {
                                //Est� no intervalo correto
                                sTempDevice.FinalThreshold = lTemp;
                                //
                                //Obt�m o THT
                                uiTemp = (unsigned int)AsciiHex2Int(sCOM2RxBuffer.Data+31);
                                if(uiTemp >= THT_MIN && uiTemp <= THT_MAX)
                                {
                                    //Est� no intervalo correto
                                    sTempDevice.AcquisitionFinalTimeout = uiTemp;
                                    //
                                    //Obt�m o time step
                                    uiTemp = (unsigned int)AsciiHex2Int(sCOM2RxBuffer.Data+35);
                                    if(uiTemp >= TIMESTEP_MIN && uiTemp <= TIMESTEP_MAX)
                                    {
                                        sTempDevice.AcquisitionTimeStep = uiTemp;
                                        //Obt�m FilterFreq
                                        uiTemp = (unsigned int)AsciiHex2Int(sCOM2RxBuffer.Data+39);
                                        if(uiTemp > FILTER_FREQ_MAX)
                                            uiTemp = 2000;
                                        if(uiTemp >= FILTER_FREQ_MIN && uiTemp <= FILTER_FREQ_MAX)
                                        {
                                            //Est� no intervalo correto
                                            sTempDevice.FilterFreq = uiTemp;
                                            sButter.fc = (double)sTempDevice.FilterFreq;
                                            ButterComputeCoef(sButter.fc);
                                            //Obt�m Sentido do aperto
                                            ucTemp2 = ByteAscii2Hex(sCOM2RxBuffer.Data+43);
                                            if(ucTemp2 == CW || ucTemp2 == CCW || ucTemp2 == CWCCW)
                                            {
                                                sTempDevice.AcquisitionExpectedDirection = ucTemp2;
                                                //Obt�m tipo da ferramenta
                                                ucTemp2 = ByteAscii2Hex(sCOM2RxBuffer.Data+45);
                                                if(ucTemp2 == TIPO_NAO_DEFINIDO ||
                                                    ucTemp2 == TIPO_APERTADEIRA_CABO ||
                                                    ucTemp2 == TIPO_APERTADEIRA_IMPULSO ||
                                                    ucTemp2 == TIPO_TORQUIMETRO_ESTALO ||
                                                    ucTemp2 == TIPO_TORQUIMETRO_DIGITAL ||
                                                    ucTemp2 == TIPO_APERTADEIRA_PENUMATICA ||
                                                    ucTemp2 == TIPO_APERTADEIRA_BATERIA ||
                                                    ucTemp2 == TIPO_APERTADEIRA_BATERIA_TRANSD ||
                                                    ucTemp2 == TIPO_APERTADEIRA_SHUT_OFF)
                                                {    
                                                    sTempDevice.ToolType = ucTemp2;
                                                    //---------
                                                    //Verifica se h� aquisi��o em curso
                                                    if(sDevice.AcquisitionStatus == ACQ_IDLE || sDevice.AcquisitionStatus == ACQ_FINISHED)
                                                    {
                                                        //Salvar valores
                                                        sDevice.FilterFreq = sTempDevice.FilterFreq;
                                                        sDevice.AcquisitionExpectedDirection = sTempDevice.AcquisitionExpectedDirection;
                                                        sDevice.AcquisitionTimeStep = sTempDevice.AcquisitionTimeStep;
                                                        sDevice.AcquisitionFinalTimeout = sTempDevice.AcquisitionFinalTimeout;
                                                        sDevice.FinalThreshold = sTempDevice.FinalThreshold;
                                                        sDevice.InitialThreshold = sTempDevice.InitialThreshold;
                                                        sDevice.ToolType = sTempDevice.ToolType;
                                                        //Libera para nova aquisi��o
                                                        sDevice.AcquisitionStatus = ACQ_IDLE;
                                                        sDevice.AcquisitionIndex = 0;
                                                        sDevice.AcquisitionPeakIndex = 0;
                                                        sDevice.AcquisitionDirection = CW;
                                                        sDevice.AcquisitionSize = 0;
                                                        sDevice.AcquisitionReady = ACQ_READY;
                                                        sDevice.AcquisitionTHFIndex = 0;
                                                        sDevice.AcquisitionMaxTorque = 0;
                                                        //teste reginaldo
                                                        if(sDevice.ToolType == TIPO_APERTADEIRA_PENUMATICA)
                                                            sTempDevice.FilterFreq = 20000;
                                                        //Teste reginaldo
                                                        ADS1271SetClock();//Altera clock do AD
                                                        
                                                        
                                                        sDevice.FastScan = 1;
                                                        ucTempBuffer[0] = 'S';
                                                        ucTempBuffer[1] = 'A';
                                                        ucTempBuffer[2] = '0';
                                                        ucTempBuffer[3] = '0';
                                                        CmdResponse(ucTempBuffer, 4);
                                                    }
                                                    else
                                                    {
                                                        //H� aquisi��o em curso
                                                        //Comando n�o aceito
                                                        ucCommandStatus = DEVICE_NOT_READY;
                                                        break;
                                                    }
                                                    //----------
                                                }
                                                else
                                                {
                                                    //N�o est� no intervalo correto
                                                    ucCommandStatus = SINTAX_ERROR;
                                                    break;
                                                }
                                            }
                                            else
                                            {
                                                //N�o est� no intervalo correto
                                                ucCommandStatus = SINTAX_ERROR;
                                                break;                                                
                                            }
                                        }
                                        else
                                        {
                                            //N�o est� no intervalo correto
                                            ucCommandStatus = SINTAX_ERROR;
                                            break;
                                        }
                                    }
                                    //
                                    else
                                    {
                                        //N�o est� no intervalo correto
                                        ucCommandStatus = SINTAX_ERROR;
                                        break;
                                    }
                                }
                                else
                                {
                                    //N�o est� no intervalo correto
                                    ucCommandStatus = SINTAX_ERROR;
                                    break;
                                }
                            }
                            else
                            {
                                //N�o est� no intervalo incorreto
                                ucCommandStatus = SINTAX_ERROR;
                                break;
                            }
                        }
                        else
                        {
                            //N�o est� no intervalo correto
                            ucCommandStatus = SINTAX_ERROR;
                            break;
                        }
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                } 
                //
                case REQUEST_NVRAM_PAGE_WRITE:
                {
                    //Foi recebida solicita��o de leitura da NVRAM
                    //extrai o n�mero de bytes
                    ucTemp2 = ByteAscii2Hex(sCOM2RxBuffer.Data+19);
                    ucTemp2 = 2 * ucTemp2 + 24;
                    if(sCOM2RxBuffer.Size == ucTemp2)
                    {
                        //extrai o endere�o inicial
                        ucTempBuffer[0] = ByteAscii2Hex(sCOM2RxBuffer.Data+15);
                        ucTempBuffer[1] = ByteAscii2Hex(sCOM2RxBuffer.Data+17);
                        uiTempVar = (ucTempBuffer[0]<<8) & 0xFF00;
                        uiTempVar |= ucTempBuffer[1];
                        if(uiTempVar >= NVRAM_MIN_ADDRESS && uiTempVar <= NVRAM_MAX_ADDRESS)
                        {
                            //Aqui o endere�o est� no intervalo correto
                            //extrai o n�mero de bytes
                            ucTemp2 = ByteAscii2Hex(sCOM2RxBuffer.Data+19);
                            if(ucTemp2 >= NVRAM_MIN_PAGE_SIZE && ucTemp2 <= NVRAM_MAX_PAGE_SIZE)
                            {
                                //Aqui o n�mero de bytes est� no intervalo correto.
                                //Verificar se n�o estourou o �ltimo endere�o da NVRAM
                                uiTempData = uiTempVar + ucTemp2 - 1;
                                if(uiTempData <= NVRAM_MAX_ADDRESS )
                                {
                                    for(ucIndex=0; ucIndex<ucTemp2; ucIndex++)
                                    {
                                        ucTempVar = ByteAscii2Hex(sCOM2RxBuffer.Data+21+ 2 * ucIndex);
                                        I2c1WriteByte(uiTempVar+ucIndex,ucTempVar);
                                    }
                                    ucTempBuffer[0] = 'P';
                                    ucTempBuffer[1] = 'W';
                                    ucTempBuffer[2] = '0';
                                    ucTempBuffer[3] = '0';
                                    CmdResponse(ucTempBuffer ,4);
                                }
                                else
                                    ucCommandStatus = SINTAX_ERROR;
                            }
                            else
                                ucCommandStatus = SINTAX_ERROR;
                        }
                        else
                            ucCommandStatus = SINTAX_ERROR;
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }                
                //
                //
                case READ_COUNTERS:
                {
                    //Foi recebida solicita��o de leitura dos contadores
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        //Atualiza valores dos contadores (NVRAM para RAM)
                        //
                        sDevice.CycleCounter = I2c1ReadLong(NVRAM_CYCLE_COUNTER);
                        sDevice.OvertorqueCounter = I2c1ReadLong(NVRAM_OVERTORQUE_COUNTER);
                        sDevice.MaxAppliedTorque = I2c1ReadLong(NVRAM_MAX_APPLIED_TORQUE); 
                        //Monta buffer de resposta
                        //
                        ucTempBuffer[0] = 'R';
                        ucTempBuffer[1] = 'C';
                        //
                        //Salva o contador total no buffer
                        //
                        ucTempVar = (unsigned char)(sDevice.CycleCounter >> 24);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2);
                        ucTempVar = (unsigned char)(sDevice.CycleCounter >> 16);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+4);
                        ucTempVar = (unsigned char)(sDevice.CycleCounter >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+6);
                        ucTempVar = (unsigned char)sDevice.CycleCounter;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+8);                        
                        //
                        //Salva o contador de sobretorque no buffer
                        //
                        ucTempVar = (unsigned char)(sDevice.OvertorqueCounter >> 24);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+10);
                        ucTempVar = (unsigned char)(sDevice.OvertorqueCounter >> 16);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+12);
                        ucTempVar = (unsigned char)(sDevice.OvertorqueCounter >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+14);
                        ucTempVar = (unsigned char)sDevice.OvertorqueCounter;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+16);
                        //
                        //Salva o torque m�ximo aplicado
                        //
                        ucTempVar = (unsigned char)(sDevice.MaxAppliedTorque >> 24);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+18);
                        ucTempVar = (unsigned char)(sDevice.MaxAppliedTorque >> 16);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+20);
                        ucTempVar = (unsigned char)(sDevice.MaxAppliedTorque >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+22);
                        ucTempVar = (unsigned char)sDevice.MaxAppliedTorque;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+24);                        
                        //
                        //Salva os bytes reservados no buffer
                        //
                        ucTempBuffer[26] = '0';
                        ucTempBuffer[27] = '0';
                        ucTempBuffer[28] = '0';
                        ucTempBuffer[29] = '0';
                        ucTempBuffer[30] = '0';
                        ucTempBuffer[31] = '0';
                        ucTempBuffer[32] = '0';
                        ucTempBuffer[33] = '0';
                        //
                        ucTempBuffer[34] = '0';
                        ucTempBuffer[35] = '0';
                        ucTempBuffer[36] = '0';
                        ucTempBuffer[37] = '0';
                        ucTempBuffer[38] = '0';
                        ucTempBuffer[39] = '0';
                        ucTempBuffer[40] = '0';
                        ucTempBuffer[41] = '0';
                        //
                        CmdResponse(ucTempBuffer, 42);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }  
                //                
                case REQUEST_NVRAM_PAGE_READ:
                {
                    //Foi recebida solicita��o de leitura da NVRAM
                    if(sCOM2RxBuffer.Size == 24)
                    {
                        //Extrai o endere�o
                        ucTempBuffer[0] = ByteAscii2Hex(sCOM2RxBuffer.Data+15);
                        ucTempBuffer[1] = ByteAscii2Hex(sCOM2RxBuffer.Data+17);
                        uiTempVar = (ucTempBuffer[0]<<8) & 0xFF00;
                        uiTempVar |= ucTempBuffer[1];
                        if(uiTempVar >= NVRAM_MIN_ADDRESS && uiTempVar <= NVRAM_MAX_ADDRESS)
                        {
                            //Aqui o endere�o est� no intervalo correto
                            //extrai o n�mero de bytes
                            ucTemp2 = ByteAscii2Hex(sCOM2RxBuffer.Data+19);
                            if(ucTemp2 >= NVRAM_MIN_PAGE_SIZE && ucTemp2 <= NVRAM_MAX_PAGE_SIZE)
                            {
                                //Aqui o n�mero de bytes est� no intervalo correto
                                //Verificar se n�o estourou o �ltimo endere�o da NVRAM
                                uiTempData = uiTempVar + ucTemp2 - 1;
                                if(uiTempData <= NVRAM_MAX_ADDRESS )
                                {
                                    //Aqui est� tudo OK com o comando
                                    //Pronto para executar
                                    //Colocar no buffer o n�mero de bytes
                                    ByteHex2Ascii(ucTemp2,ucTempBuffer+2);
                                    for(ucIndex = 0;ucIndex<ucTemp2;ucIndex++)
                                    {
                                        ucTempVar = I2c1ReadByte(uiTempVar+ucIndex);
                                        ByteHex2Ascii(ucTempVar,ucTempBuffer+4+2*ucIndex);
                                    }
                                    ucTempBuffer[0] = 'P';
                                    ucTempBuffer[1] = 'R';
                                    CmdResponse(ucTempBuffer ,2*ucTemp2+4);
                                }
                                else
                                    ucCommandStatus = SINTAX_ERROR;
                            }
                            else
                                ucCommandStatus = SINTAX_ERROR;
                        }
                        else
                            ucCommandStatus = SINTAX_ERROR;
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }                
                //
                case CALIB_WRITE:
                {
                    //Foi recebida solicita��o para gravar valor de calibra��o
                    if(sCOM2RxBuffer.Size == 58)
                    {
                        //O size est� correto
                        //Obt�m a sensibilidade do torque
                        ulTempBuffer[0] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+15);
                        ulTempBuffer[1] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+17);
                        ulTempBuffer[2] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+19);
                        ulTempBuffer[3] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+21);
                        sDevice.TorqueSensitivity = (ulTempBuffer[0]<<24) & 0xFF000000;
                        sDevice.TorqueSensitivity |= (ulTempBuffer[1]<<16) & 0xFFFF0000;
                        sDevice.TorqueSensitivity |= (ulTempBuffer[2]<<8) & 0xFFFFFF00;
                        sDevice.TorqueSensitivity |= ulTempBuffer[3];
                        //Obt�m a sensibilidade do �ngulo
                        ulTempBuffer[0] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+23);
                        ulTempBuffer[1] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+25);
                        ulTempBuffer[2] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+27);
                        ulTempBuffer[3] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+29);
                        sDevice.AngleSensivity = (ulTempBuffer[0]<<24) & 0xFF000000;
                        sDevice.AngleSensivity |= (ulTempBuffer[1]<<16) & 0xFFFF0000;
                        sDevice.AngleSensivity |= (ulTempBuffer[2]<<8) & 0xFFFFFF00;
                        sDevice.AngleSensivity |= ulTempBuffer[3];
                        //Obt�m o timestamp
                        ulTempBuffer[0] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+31);
                        ulTempBuffer[1] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+33);
                        ulTempBuffer[2] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+35);
                        ulTempBuffer[3] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+37);
                        sDevice.CalibTimeStamp = (ulTempBuffer[0]<<24) & 0xFF000000;
                        sDevice.CalibTimeStamp |= (ulTempBuffer[1]<<16) & 0xFFFF0000;
                        sDevice.CalibTimeStamp |= (ulTempBuffer[2]<<8) & 0xFFFFFF00;
                        sDevice.CalibTimeStamp |= ulTempBuffer[3];
                        //Obt�m o campo de dados
                        ulTempBuffer[0] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+39);
                        ulTempBuffer[1] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+41);
                        ulTempBuffer[2] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+43);
                        ulTempBuffer[3] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+45);
                        sDevice.CalibData = (ulTempBuffer[0]<<24) & 0xFF000000;
                        sDevice.CalibData |= (ulTempBuffer[1]<<16) & 0xFFFF0000;
                        sDevice.CalibData |= (ulTempBuffer[2]<<8) & 0xFFFFFF00;
                        sDevice.CalibData |= ulTempBuffer[3]; 
                        //
                        //Grava na NVRAM
                        I2c1WriteLong(NVRAM_TORQUE_SENSITIVITY, sDevice.TorqueSensitivity);
                        I2c1WriteLong(NVRAM_ANGLE_SENSITIVITY, sDevice.AngleSensivity);
                        I2c1WriteLong(NVRAM_CALIB_TIMESTAMP, sDevice.CalibTimeStamp);
                        I2c1WriteLong(NVRAM_CALIB_DATA, sDevice.CalibData);
                        //
                        sDevice.TorqueSensitivity = I2c1ReadLong(NVRAM_TORQUE_SENSITIVITY);
                        sDevice.AngleSensivity = I2c1ReadLong(NVRAM_ANGLE_SENSITIVITY);
                        sDevice.CalibTimeStamp = I2c1ReadLong(NVRAM_CALIB_TIMESTAMP);
                        sDevice.CalibData = I2c1ReadLong(NVRAM_CALIB_DATA);
                        //
                        //Envia respsota de confirma��o
                        ucTempBuffer[0] = 'C';
                        ucTempBuffer[1] = 'W';
                        ucTempBuffer[2] = '0';
                        ucTempBuffer[3] = '0';
                        CmdResponse(ucTempBuffer ,4);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }                
                //
                case WRITE_CAPACITY:
                {
                    //Grava capacidade do transdutor
                    if(sCOM2RxBuffer.Size == 24)
                    {
                        I2c1WriteByte(NVRAM_CAPACITY,sCOM2RxBuffer.Data[15]);
                        I2c1WriteByte(NVRAM_CAPACITY+1,sCOM2RxBuffer.Data[16]);
                        I2c1WriteByte(NVRAM_CAPACITY+2,sCOM2RxBuffer.Data[17]);
                        I2c1WriteByte(NVRAM_CAPACITY+3,sCOM2RxBuffer.Data[18]);
                        I2c1WriteByte(NVRAM_CAPACITY+4,sCOM2RxBuffer.Data[19]);
                        I2c1WriteByte(NVRAM_CAPACITY+5,sCOM2RxBuffer.Data[20]);
                        for(ucIndex = 0; ucIndex < CAPACITY_SIZE; ucIndex++)
                            sDevice.Capacity[ucIndex] = I2c1ReadByte(NVRAM_CAPACITY+ucIndex);    
                        StringCapacity2Num();
                        l64Temp = sDevice.NominalTorque * 1000000000000;
                        l64Temp = l64Temp / TORQUE_FULL_NOMINAL_SCALE;
                        sDevice.TorqueSensitivity = (unsigned long)l64Temp;
                        I2c1WriteLong(NVRAM_TORQUE_SENSITIVITY, sDevice.TorqueSensitivity);
                        ucTempBuffer[0] = 'X';
                        ucTempBuffer[1] = 'C';
                        ucTempBuffer[2] = '0';
                        ucTempBuffer[3] = '0';                  
                        CmdResponse(ucTempBuffer, 4);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }   
                // 
                case WRITE_MODEL:
                {
                    //Grava capacidade do transdutor
                    if(sCOM2RxBuffer.Size == 50)
                    {
                        for(ucIndex = 0; ucIndex < MODEL_SIZE; ucIndex++)
                        {
                            I2c1WriteByte(NVRAM_MODEL + ucIndex,sCOM2RxBuffer.Data[ucIndex+15]);  
                        }
                        for(ucIndex = 0; ucIndex < MODEL_SIZE; ucIndex++)
                            sDevice.Model[ucIndex] = I2c1ReadByte(NVRAM_MODEL+ucIndex);                        
                        ucTempBuffer[0] = 'X';
                        ucTempBuffer[1] = 'M';
                        ucTempBuffer[2] = '0';
                        ucTempBuffer[3] = '0';                  
                        CmdResponse(ucTempBuffer, 4);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }   
                //  
                case WRITE_SERIALNUMBER:
                {
                    //Grava capacidade do transdutor
                    if(sCOM2RxBuffer.Size == 26)
                    {
                        for(ucIndex = 0; ucIndex < SERIAL_SIZE; ucIndex++)
                        {
                            I2c1WriteByte(NVRAM_SERIAL_NUMBER + ucIndex,sCOM2RxBuffer.Data[ucIndex+15]);  
                        }
                        for(ucIndex = 0; ucIndex < SERIAL_SIZE; ucIndex++)
                            sDevice.SerialNumber[ucIndex] = I2c1ReadByte(NVRAM_SERIAL_NUMBER+ucIndex);                        
                        ucTempBuffer[0] = 'X';
                        ucTempBuffer[1] = 'S';
                        ucTempBuffer[2] = '0';
                        ucTempBuffer[3] = '0';                  
                        CmdResponse(ucTempBuffer, 4);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }   
                //  
                case WRITE_BT_CONFIG:
                {
                    //Entra no modo transparente para o bluetooth
                    //Testa de est� no tamanho correto
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        //Atualiza o baud rate
                        ucTransparentRxCounter = 0;
                        ucTransparentEscape = 0;
                        COM1Disable();
                        COM2Disable();
                        #if(HC06_CONN == 0)
                            COM2Init(BAUD_38400);
                            COM1Init(BAUD_38400);  
                            //Desliga m�dulo bluetooth
                            BLUETOOTH_PWR_ENABLE_PIN = OFF;
                            TimerLoad(DELAYTIMER,1000,RUN);
                            while(TimerStatus(DELAYTIMER)==RUN)
                            {
                                PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                                ClrWdt();
                            }
                            //Religa m�dulo bluetooth
                            BLUETOOTH_PWR_ENABLE_PIN = ON;
                            //Aguarda boot do m�dulo
                            TimerLoad(DELAYTIMER,1000,RUN);
                            while(TimerStatus(DELAYTIMER)==RUN)
                            {
                                PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                                ClrWdt();
                            }                            
                        #elif(HC06_CONN == 1)
                            COM2Init(BAUD_38400);
                            COM1Init(BAUD_38400); 
                            //Desliga m�dulo bluetooth
                            BLUETOOTH_PWR_ENABLE_PIN = OFF;
                            TimerLoad(DELAYTIMER,1000,RUN);
                            while(TimerStatus(DELAYTIMER)==RUN)
                            {
                                PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                                ClrWdt();
                            }
                            //Religa m�dulo bluetooth
                            BLUETOOTH_PWR_ENABLE_PIN = ON;
                            //Aguarda boot do m�dulo
                            TimerLoad(DELAYTIMER,1000,RUN);
                            while(TimerStatus(DELAYTIMER)==RUN)
                            {
                                PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                                ClrWdt();
                            }                            
                        #elif(HC06_CONN == 2)
                            COM2Init(BAUD_115200);
                            COM1Init(BAUD_115200);
                            BLUETOOTH_PWR_ENABLE_PIN = ESP_POWER_OFF;  //Desliga m�dulo
                            TimerLoad(DELAYTIMER,300,RUN);
                            ESP8266_MODE_PIN = OFF;
                            while(TimerStatus(DELAYTIMER)==RUN)
                            {
                                PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                                ClrWdt();
                            }
                            BLUETOOTH_PWR_ENABLE_PIN = ESP_POWER_ON;  //Religa m�dulo
                            TimerLoad(DELAYTIMER,300,RUN);
                            while(TimerStatus(DELAYTIMER)==RUN)
                            {
                                PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                                ClrWdt();
                            }  
                            ESP8266_MODE_PIN = ON;
                        #elif(HC06_CONN == 3)
                            COM2Init(BAUD_38400);
                            COM1Init(BAUD_38400);  
                            //Desliga m�dulo bluetooth
                            BLUETOOTH_PWR_ENABLE_PIN = OFF;
                            TimerLoad(DELAYTIMER,1000,RUN);
                            while(TimerStatus(DELAYTIMER)==RUN)
                            {
                                PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                                ClrWdt();
                            }
                            //Religa m�dulo bluetooth
                            BLUETOOTH_PWR_ENABLE_PIN = ON;
                            //Aguarda boot do m�dulo
                            TimerLoad(DELAYTIMER,1000,RUN);
                            while(TimerStatus(DELAYTIMER)==RUN)
                            {
                                PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                                ClrWdt();
                            } 
                        #endif    
                        //Alterna para o modo transparente
                        sDevice.BlueToothTrasparent = BT_TRANSPARENT_MODE;
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }   
                // 
                case CLEAR_COUNTER:
                {
                    //Zera contador de ciclos
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        sDevice.CycleCounter = 0;
                        sDevice.OvertorqueCounter = 0;
                        sDevice.MaxAppliedTorque = 0;
                        I2c1WriteLong(NVRAM_CYCLE_COUNTER, sDevice.CycleCounter);
                        I2c1WriteLong(NVRAM_OVERTORQUE_COUNTER, sDevice.OvertorqueCounter);
                        I2c1WriteLong(NVRAM_MAX_APPLIED_TORQUE, sDevice.MaxAppliedTorque);
                        ucTempBuffer[0] = 'X';
                        ucTempBuffer[1] = 'T';
                        ucTempBuffer[2] = '0';
                        ucTempBuffer[3] = '0';                  
                        CmdResponse(ucTempBuffer, 4);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }  
                //   
                case WRITE_CLICK_SETUP:
                {
                    //Configura sensibilidade para torquimetro de estalo
                    if(sCOM2RxBuffer.Size == 24)
                    {
                        //Fall 
                        ulTempBuffer[0] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+15);
                        //Rise
                        ulTempBuffer[1] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+17);
                        //Width
                        ulTempBuffer[2] = (long)ByteAscii2Hex(sCOM2RxBuffer.Data+19);
                        //
                        if(ulTempBuffer[0] >= MIN_CLICK_FALL && ulTempBuffer[0] <= MAX_CLICK_FALL)
                        {
                            if(ulTempBuffer[1] >= MIN_CLICK_RISE && ulTempBuffer[1] <= MAX_CLICK_RISE)
                            {
                                if(ulTempBuffer[2] >= MIN_CLICK_RISE && ulTempBuffer[2] <= MAX_CLICK_RISE)
                                {
                                    //Todos os par�metros est�o nos intervalos corretos
                                    sDevice.ClickFall = (unsigned int)ulTempBuffer[0];
                                    sDevice.ClickRise = (unsigned int)ulTempBuffer[1];
                                    sDevice.ClickWidth = (unsigned int)ulTempBuffer[2];
                                    I2c1WriteWord(NVRAM_CLICK_FALL, sDevice.ClickFall);
                                    I2c1WriteWord(NVRAM_CLICK_RISE, sDevice.ClickRise);
                                    I2c1WriteWord(NVRAM_CLICK_WITDH, sDevice.ClickWidth);
                                    ucTempBuffer[0] = 'C';
                                    ucTempBuffer[1] = 'S';
                                    ucTempBuffer[2] = '0';
                                    ucTempBuffer[3] = '0';                  
                                    CmdResponse(ucTempBuffer, 4);                                    
                                }
                                else
                                {
                                   ucCommandStatus = SINTAX_ERROR; 
                                }
                            }
                            else
                            {
                                ucCommandStatus = SINTAX_ERROR; 
                            }
                        }
                        else
                        {
                            ucCommandStatus = SINTAX_ERROR; 
                        }

                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }  
                //  
                // 
                case READ_CLICK_SETUP:
                {
                    //Obt�m a sensibilidade atual para torquimetro de estalo
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        //
                        ucTempBuffer[0] = 'C';
                        ucTempBuffer[1] = 'R';
                        //
                        //Salva o conte�do no buffer
                        //
                        ucTempVar = (unsigned char)(sDevice.ClickFall);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2);                        
                        //
                        ucTempVar = (unsigned char)(sDevice.ClickRise);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+4); 
                        //
                        ucTempVar = (unsigned char)(sDevice.ClickWidth);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+6);  
                        //
                        CmdResponse(ucTempBuffer, 8);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                } 
                // 
                case WRITE_ACQ_CONFIG_3:
                {
                    //Configura��es adicionais da aquisi��o
                    if(sCOM2RxBuffer.Size == 74)
                    {
                        //Obt�m o Delay Time
                        uiTemp = (unsigned int)AsciiHex2Int(sCOM2RxBuffer.Data+15);
                        if(uiTemp >= ACQ_DELAY_MIN && uiTemp <= ACQ_DELAY_MAX)
                        {
                            sTempDevice.AcquisitionDelayTimeout = uiTemp;
                            //
                            //Obt�m o Reset Time
                            uiTemp = (unsigned int)AsciiHex2Int(sCOM2RxBuffer.Data+19);
                            if(uiTemp >= ACQ_RESET_TIMEOUT_MIN && uiTemp <= ACQ_RESET_TIMEOUT_MAX)
                            {
                                sTempDevice.AcquisitionResetTimeout = uiTemp;
                                sDevice.AcquisitionDelayTimeout = sTempDevice.AcquisitionDelayTimeout;
                                sDevice.AcquisitionResetTimeout = sTempDevice.AcquisitionResetTimeout;
                                ucTempBuffer[0] = 'S';
                                ucTempBuffer[1] = 'C';
                                ucTempBuffer[2] = '0';
                                ucTempBuffer[3] = '0';                  
                                CmdResponse(ucTempBuffer, 4);                                 
                            }
                            else
                            {
                                ucCommandStatus = SINTAX_ERROR;
                            }
                        }
                        else
                        {
                            ucCommandStatus = SINTAX_ERROR;
                        }
                    } 
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }
                //
                case READ_ACQ_CONFIG_3:
                {
                    //Obt�m a configura��es adicionais de aquisi��o 2
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        //
                        ucTempBuffer[0] = 'G';
                        ucTempBuffer[1] = 'C';
                        //
                        //Salva o conte�do no buffer
                        //
                        //Obt�m o delay Time
                        ucTempVar = (unsigned char)(sDevice.AcquisitionDelayTimeout >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2);
                        ucTempVar = (unsigned char)sDevice.AcquisitionDelayTimeout;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+4);
                        //Obt�m o reset Time
                        ucTempVar = (unsigned char)(sDevice.AcquisitionResetTimeout >> 8);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+6);
                        ucTempVar = (unsigned char)sDevice.AcquisitionResetTimeout;
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+8);                        
                        //
                        //Obt�m os bytes reservados
                        for(i=10;i<58;i++)
                            ucTempBuffer[i] = '0'; 
                        CmdResponse(ucTempBuffer, 58);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }  
                //
                case WRITE_TORQUE_OFFSET:
                {
                    //For�a valor de offset do torque
                    if(sCOM2RxBuffer.Size == 18)
                    {
                        //
                        ucTempBuffer[0] = 'C';
                        ucTempBuffer[1] = 'R';
                        //
                        //Salva o conte�do no buffer
                        //
                        ucTempVar = (unsigned char)(sDevice.ClickFall);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+2);                        
                        //
                        ucTempVar = (unsigned char)(sDevice.ClickRise);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+4); 
                        //
                        ucTempVar = (unsigned char)(sDevice.ClickWidth);
                        ByteHex2Ascii(ucTempVar,ucTempBuffer+6);  
                        //
                        CmdResponse(ucTempBuffer, 8);
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                } 
                //
                case GET_DATA_ASCII:
                {
                    //Foi recebida solicita��o de leitura dos pontos armazenados
                    if(sCOM2RxBuffer.Size == 26)
                    {
                        if(sDevice.AcquisitionReady == ACQ_READY || sDevice.AcquisitionStatus != ACQ_IDLE)
                        {
                            //Verifica se o endere�o index inicial � v�lido
                            uiTemp = (unsigned int)AsciiHex2Int(sCOM2RxBuffer.Data+15);
                            if(uiTemp < RAM_BUF_SIZE)
                            {
                                sSendLog.StartIndex = uiTemp;
                                //Verifica se o Size est� no intervalo correto
                                ucTemp = ByteAscii2Hex(sCOM2RxBuffer.Data+19);
                                if(ucTemp > 0 && ucTemp <= MAX_GET_DATA_SIZE)
                                {
                                    sSendLog.Size = ucTemp;
                                    //Verifica se o passo est� no intervalo correto
                                    ucTemp = ByteAscii2Hex(sCOM2RxBuffer.Data+21);
                                    if(ucTemp > 0 && ucTemp <= sSendLog.Size)
                                    {
                                        sSendLog.Step = ucTemp - 1;
                                        //Monta o buffer de resposta
                                        for(i = 0; i < sSendLog.Size; i++)
                                        {
                                                if(i==0)
                                                    Registry2BufferAscii(sSendLog.StartIndex + i,ucTempBuffer+2+i*6);
                                                else
                                                {
                                                    Registry2BufferAscii(sSendLog.StartIndex + i + sSendLog.Step * i,ucTempBuffer+2+i*6);
                                                }
                                        }
                                        ucTempBuffer[0] ='G';
                                        ucTempBuffer[1] ='X';
                                        CmdResponse(ucTempBuffer, (sSendLog.Size * 6)+2);
                                        //CmdGraphResponse(sSendLog.Size * 5);
                                    }
                                    else
                                    {
                                        //Passo incorreto
                                        ucCommandStatus = SINTAX_ERROR;
                                        break;                                
                                    }                                     
                                }
                                else
                                {
                                    //Size incorreto
                                    ucCommandStatus = SINTAX_ERROR;
                                    break;                                
                                }                                
                            }
                            else
                            {
                                //Endere�o inicial inv�lido
                                ucCommandStatus = SINTAX_ERROR;
                                break;                                
                            }

                        }
                        else
                        {
                            //Comando n�o aceito
                            //Aquisi��o n�o liberada/configurada
                            ucCommandStatus = DEVICE_NOT_READY;
                            break;                            
                        }
                    }
                    else
                        ucCommandStatus = SINTAX_ERROR;
                    break;
                }                 
                //
                default:
                {
                    ucCommandStatus = INVALID_CMD;
                    break;
                }
            }
        }
        else
        {
            //Verifica se n�o � broadcast
            for(i = 0; i < 12; i++)
            {
                ucTempAscii[i] = '0';
                ucTempAscii2[i] = '0';
            }
            ByteHex2Ascii(sDevice.ExpAddress, ucTempAscii2);
            if((BufferCompare(sCOM2RxBuffer.Data+1, ucTempAscii, 12) == TRUE) || (BufferCompare(sCOM2RxBuffer.Data+1, ucTempAscii2, 12) == TRUE))
            {
                //� ID de broadcast
                //Devolver ID
                //Identificar o comando
                uiCommand = (sCOM2RxBuffer.Data[13] <<8) & 0b1111111100000000;
                uiCommand |= sCOM2RxBuffer.Data[14];
                switch(uiCommand)
                {
                    case REQUEST_ID:
                    {
                        //Foi recebida solicita��o de torque
                        if(sCOM2RxBuffer.Size == 18)
                        {
                        TimerLoad(DELAYTIMER,5 + (25 * sDevice.ExpAddress),RUN);
                        while(TimerStatus(DELAYTIMER)==RUN)
                        {
                            PWR_ENABLE_PIN = ~PWR_ENABLE_PIN;
                            ClrWdt();
                        }
                            ucTempBuffer[0] = 'I';
                            ucTempBuffer[1] = 'D';
                            CmdResponse(ucTempBuffer, 2);
                        }
                        else
                            ucCommandStatus = SINTAX_ERROR;
                        if(ucUartSel == COM2)
                            sCOM2RxBuffer.Status = SLOWCOMM_IDLE;
                        if(ucUartSel == COM1)
                            sCOM1RxBuffer.Status = SLOWCOMM_IDLE;
                        break;
                    }
                    case RESET_CMD:
                    {
                        //Foi recebida solicita��o de torque
                        if(sCOM2RxBuffer.Size == 18)
                        {
                            //Inciar procedimento de reset
                            __asm__("RESET");
                        }
                        break;
                    }  
                }
                return;
            }
            else
            {
                //o ID n�o confere
                if(ucUartSel == COM2)
                    sCOM2RxBuffer.Status = SLOWCOMM_IDLE;
                if(ucUartSel == COM1)
                    sCOM1RxBuffer.Status = SLOWCOMM_IDLE;
                return;
            }
        }
    }
    else
    {
        //Aqui ocorreu um erro de CRC
        //Enviar resposta de erro de CRC
        ucTempBuffer[0] = 'E';
        ucTempBuffer[1] = 'R';
        ucTempBuffer[2] = '0';
        ucTempBuffer[3] = '1';
        CmdResponse(ucTempBuffer ,4);
    }
    if(ucCommandStatus == SINTAX_ERROR)
    {
        //Aqui ocorreu um erro de sintaxe
        //Enviar resposta de erro de Sintaxe
        ucTempBuffer[0] = 'E';
        ucTempBuffer[1] = 'R';
        ucTempBuffer[2] = '0';
        ucTempBuffer[3] = '2';
        CmdResponse(ucTempBuffer ,4);
    }
    //
    if(ucCommandStatus == INVALID_CMD)
    {
        //Aqui ocorreu um erro de sintaxe
        //Enviar resposta de erro de Sintaxe
        ucTempBuffer[0] = 'E';
        ucTempBuffer[1] = 'R';
        ucTempBuffer[2] = '0';
        ucTempBuffer[3] = '3';
        CmdResponse(ucTempBuffer ,4);
    }
//    
    if(ucCommandStatus == DEVICE_NOT_READY)
    {
        //Aqui ocorreu um erro de sintaxe
        //Enviar resposta de erro de Sintaxe
        ucTempBuffer[0] = 'E';
        ucTempBuffer[1] = 'R';
        ucTempBuffer[2] = '0';
        ucTempBuffer[3] = '4';
        CmdResponse(ucTempBuffer ,4);
    }
    if(ucUartSel == COM2)
        sCOM2RxBuffer.Status = SLOWCOMM_IDLE;
    if(ucUartSel == COM1)
        sCOM1RxBuffer.Status = SLOWCOMM_IDLE;
}
//
//
unsigned char BufferCompare(unsigned char *ucBuf1,unsigned char *ucBuf2,unsigned char ucSize)
{
    unsigned char ucTemp;
    for(ucTemp = 0;ucTemp < ucSize; ucTemp++)
    {
        if(ucBuf1[ucTemp] != ucBuf2[ucTemp])
        {
            return FALSE;
        }
    }
    return TRUE;
}
//
unsigned int CheckSum(unsigned int *ptrBuffer, unsigned char ucSize)
{
    //Calcula o checksum do intervalo especificado
    unsigned int uiCheckSum; //uiTemp;
    unsigned char ucIndex;
    uiCheckSum = 0;
    for(ucIndex = 0; ucIndex < ucSize; ucIndex++)
    {
        uiCheckSum ^= *ptrBuffer;
        ptrBuffer++;
    }
    return uiCheckSum;
}
//
//void ParametersCheckSumRefresh(void)
//{
//    unsigned int uiTemp;
//    unsigned char ucIndex, ucSize;
//    ucSize = NVRAM_PARAMETERS_SIZE/2;
//    for(ucIndex = 0; ucIndex < ucSize; ucIndex++)
//    {
//        uiTemp = I2c1ReadByte(2*ucIndex+NVRAM_PARAMETERS_START);
//        uiTempBuffer[ucIndex] = uiTemp <<8;
//        uiTemp = I2c1ReadByte(2*ucIndex+NVRAM_PARAMETERS_START+1);
//        uiTempBuffer[ucIndex] |= uiTemp;
//    }
//    uiTemp = CheckSum(uiTempBuffer,ucSize);
//    I2c1WriteByte(NVRAM_CHECKSUM+1,(unsigned char)uiTemp);
//    I2c1WriteByte(NVRAM_CHECKSUM,uiTemp>>8);
//}
//
long Ascii2Long(unsigned char *ptrAscii)
{
    long ulTemp = 0;
    long ulTemp2 = 0;
    ClrWdt();                       //Reset do C�o de guarda
    ulTemp2 = *ptrAscii;
    ulTemp2 -= 0x30;
    ulTemp2 = ulTemp2 * 100000;
    ulTemp += ulTemp2;
    ptrAscii++;

    ulTemp2 = *ptrAscii;
    ulTemp2 -= 0x30;
    ulTemp2 = ulTemp2 * 10000;
    ulTemp += ulTemp2;
    ptrAscii++;

    ulTemp2 = *ptrAscii;
    ulTemp2 -= 0x30;
    ulTemp2 = ulTemp2 * 1000;
    ulTemp += ulTemp2;
    ptrAscii++;

    ulTemp2 = *ptrAscii;
    ulTemp2 -= 0x30;
    ulTemp2 = ulTemp2 * 100;
    ulTemp += ulTemp2;
    ptrAscii++;

    ulTemp2 = *ptrAscii;
    ulTemp2 -= 0x30;
    ulTemp2 = ulTemp2 * 10;
    ulTemp += ulTemp2;
    ptrAscii++;

    ulTemp2 = *ptrAscii;
    ulTemp2 -= 0x30;
    ulTemp += ulTemp2;
    return ulTemp;
}
//
long AsciiHex2Long(unsigned char *ptrAscii)
{
    long lData = 0;
    long lTemp[4];
    ClrWdt();                       //Reset do C�o de guarda
    lTemp[0] = (long)ByteAscii2Hex(ptrAscii);
    lTemp[1] = (long)ByteAscii2Hex(ptrAscii+2);
    lTemp[2] = (long)ByteAscii2Hex(ptrAscii+4);
    lTemp[3] = (long)ByteAscii2Hex(ptrAscii+6);    
    //
    lData = (lTemp[0]<<24) & 0xFF000000;
    lData |= (lTemp[1]<<16) & 0xFFFF0000;
    lData |= (lTemp[2]<<8) & 0xFFFFFF00;
    lData |= lTemp[3];
    return lData;
}
//
int AsciiHex2Int(unsigned char *ptrAscii)
{
    int iData = 0;
    int iTemp[2];
    ClrWdt();                       //Reset do C�o de guarda
    iTemp[0] = (int)ByteAscii2Hex(ptrAscii);
    iTemp[1] = (int)ByteAscii2Hex(ptrAscii+2);
    //
    iData = (iTemp[0]<<8) & 0xFF00;
    iData |= iTemp[1];
    return iData;
}
//
void BlueToothCmd(unsigned char *ucCmd, unsigned char ucCmdSize)
{
    unsigned char ucTemp;
    for(ucTemp = 0;ucTemp < ucCmdSize; ucTemp++)
    {
        sCOM1TxBuffer.Data[ucTemp] = ucCmd[ucTemp];   //Move primeiro byte do comando
    } 
    //sCOM1TxBuffer.Data[ucCmdSize] = 13;
    //sCOM1TxBuffer.Data[ucCmdSize+1] = 10;
    sCOM1TxBuffer.Pointer = 0;                      //Aponta para o primeiro byte a ser transmitido
    sCOM1TxBuffer.Size = ucCmdSize;//+2;               //Determina o tamanho do buffer
    COM1TxStart();                               //Dispara a transmiss�o pela UART
}
