#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "tool.h"

SerialRxBufferStruct sCOM2RxBuffer;
SerialTxBufferStruct sCOM2TxBuffer;
//
SerialRxBufferStruct sCOM1RxBuffer;
SerialTxBufferStruct sCOM1TxBuffer;
//
extern DeviceStruct sDevice;
//
unsigned char ucUartSel = 0;

unsigned char ucTransparentRxBuffer[5];
unsigned char ucTransparentRxCounter = 0;
unsigned char ucTransparentRxData;
unsigned char ucTransparentEscape = 0;

//
void EnableLowSpeedTx(void)
{
    INTCON2bits.GIE = 0;                            //Desabilita todas as INTS
    __builtin_write_OSCCONL(OSCCON & ~(1<<6));      //Destrava IO Lock bit
    COM2_TX_REMAP_PIN;
    __builtin_write_OSCCONL(OSCCON | (1<<6));       //Trava IO Lock bit
    INTCON2bits.GIE = 1;                            //Habilita todas as INTS
}
//
//
void EnableHiSpeedTx(void)
{
    INTCON2bits.GIE = 0;                            //Desabilita todas as INTS
    __builtin_write_OSCCONL(OSCCON & ~(1<<6));      //Destrava IO Lock bit
//    RS485_HISPEED_TX_PIN;
    __builtin_write_OSCCONL(OSCCON | (1<<6));       //Trava IO Lock bit
    INTCON2bits.GIE = 1;                            //Habilita todas as INTS
}
//
//
//
void COM1Disable(void)
{
    IEC0bits.U1TXIE = 0;            //Habilita interrupção de TX
    IEC0bits.U1RXIE = 0;            //Habilita interrupção de RX
    U1MODEbits.UARTEN = 0;          //UART Habilitada    
}
//
void COM2Disable(void)
{
    IEC1bits.U2TXIE = 0;            //Habilita interrupção de TX
    IEC1bits.U2RXIE = 0;            //Habilita interrupção de RX
    U2MODEbits.UARTEN = 0;          //UART Habilitada   
}

void COM1Init(unsigned int uiBaudRate)
{
    U1MODEbits.UARTEN = 1;          //UART habilitada
    U1MODEbits.USIDL = 1;           //Discontinue module operation when device enters Idle mode
    U1MODEbits.IREN = 0;            //IRDA encoder desabilitado
    U1MODEbits.RTSMD = 0;           //RTS em modo Flow Control
    U1MODEbits.UEN = 0x00;          //Habilita TX, RX e RTS
    U1MODEbits.WAKE = 0;            //RX Wake-up desabilitado
    U1MODEbits.LPBACK = 0;          //Loopback desabilitado
    U1MODEbits.ABAUD = 0;           //Auto-Baud Desabilitado
    U1MODEbits.URXINV = 0;          //RX idle = 1
    U1MODEbits.BRGH = 0;            //Baud Rate Generator no modo 16X
    U1MODEbits.PDSEL = 0x00;        //8bits, sem paridade
    U1MODEbits.STSEL = 0;           //1 Stop Bit
    U1BRG = uiBaudRate;         //Configura o baud rate
#if HC06_CONFIG == 1
    U1BRG = BT_CONFIG_COMM_BAUD;    //Configura o baud rate
#endif    
    U1STAbits.UTXISEL1 = 0;         //
    U1STAbits.UTXISEL0 = 1;         //Gera interrupt após esvaziar FIFO
    U1STAbits.UTXINV = 0;           //TX idle = 1
    U1STAbits.UTXBRK = 0;           //Não envia Break bit
    U1STAbits.UTXEN = 1;            //Habilita pino de TX
    U1STAbits.URXISEL = 0x00;       //Gera interrupt para cada caractere recebido
    U1STAbits.ADDEN = 0;            //Address Detect desabilitado
//    
    IEC0bits.U1TXIE = 1;            //Habilita interrupção de TX
    IEC0bits.U1RXIE = 1;            //Habilita interrupção de RX
    U1MODEbits.UARTEN = 1;          //UART Habilitada
    sCOM1RxBuffer.Pointer = 0;
    sCOM1RxBuffer.Status = UART_IDLE;
    sCOM1TxBuffer.Status = UART_EMPTY;
}
//
void COM2Init(unsigned int uiBaudRate)
{
    U2MODEbits.UARTEN = 1;          //UART habilitada
    U2MODEbits.USIDL = 1;           //Discontinue module operation when device enters Idle mode
    U2MODEbits.IREN = 0;            //IRDA encoder desabilitado
    U2MODEbits.RTSMD = 0;           //RTS em modo Flow Control
    U2MODEbits.UEN = 0x00;          //Habilita TX, RX e RTS
    U2MODEbits.WAKE = 0;            //RX Wake-up desabilitado
    U2MODEbits.LPBACK = 0;          //Loopback desabilitado
    U2MODEbits.ABAUD = 0;           //Auto-Baud Desabilitado
    U2MODEbits.URXINV = 0;          //RX idle = 1
    U2MODEbits.BRGH = 0;            //Baud Rate Generator no modo 16X
    U2MODEbits.PDSEL = 0x00;        //8bits, sem paridade
    U2MODEbits.STSEL = 0;           //1 Stop Bit
    U2BRG = uiBaudRate;
    U2STAbits.UTXISEL1 = 0;         //
    U2STAbits.UTXISEL0 = 1;         //Gera interrupt após esvaziar FIFO
    U2STAbits.UTXINV = 0;           //TX idle = 1
    U2STAbits.UTXBRK = 0;           //Não envia Break bit
    U2STAbits.UTXEN = 1;            //Habilita pino de TX
    U2STAbits.URXISEL = 0x00;       //Gera interrupt para cada caractere recebido
    U2STAbits.ADDEN = 0;            //Address Detect desabilitado
    IEC1bits.U2TXIE = 1;            //Habilita interrupção de TX
    IEC1bits.U2RXIE = 1;            //Habilita interrupção de RX
    U2MODEbits.UARTEN = 1;          //UART Habilitada
    sCOM2RxBuffer.Pointer = 0;
    sCOM2RxBuffer.Status = UART_IDLE;
    sCOM2TxBuffer.Status = SLOWCOMM_TX_EMPTY;
}
void __attribute__((__interrupt__,no_auto_psv)) _U2RXInterrupt(void)
{
    if(sDevice.BlueToothTrasparent == BT_NORMAL_MODE)
    {
        sCOM2RxBuffer.Data[sCOM2RxBuffer.Pointer] = U2RXREG;            //Obtém byte recebido pela UART
        switch(sCOM2RxBuffer.Status)
        {
            case SLOWCOMM_IDLE:
            {
                //Aqui está esperando por um STX para iniciar o armazenamento no buffer
                if(sCOM2RxBuffer.Data[0] == STX)
                {
                    //Aqui foi recebido um STX
                    //Apontar para o início do buffer de armazenamento
                    //Mudar para estado SLOWCOMM_STX_RECEIVED
                    sCOM2RxBuffer.Pointer = 1;        //Aponta para o primeiro byte de dados no buffer
                    sCOM2RxBuffer.Status = SLOWCOMM_STX_RECEIVED;
                    sCOM2RxBuffer.Size = 1;
                 }
                break;
            }
            //
            case SLOWCOMM_STX_RECEIVED:
            {
                if(sCOM2RxBuffer.Data[sCOM2RxBuffer.Pointer] != ETX)
                {
                    if(sCOM2RxBuffer.Data[sCOM2RxBuffer.Pointer] == STX)
                    {
                        //Aqui foi recebido um STX. Fazer Resync.
                        sCOM2RxBuffer.Data[0] = sCOM2RxBuffer.Data[sCOM2RxBuffer.Pointer];
                        sCOM2RxBuffer.Pointer = 1;
                        sCOM2RxBuffer.Size = 1;
                    }
                    else
                    {
                        //Aqui foi recebido um dado válido
                        //Verificar se há espaço disponível no buffer
                        if(sCOM2RxBuffer.Pointer < SLOW_RX_MAX_BUF_SIZE)
                        {
                            //Ainda há espaço no buffer
                            //Armazenar dado
                            sCOM2RxBuffer.Pointer++;        //Incrementa ponteiro
                            sCOM2RxBuffer.Size++;           //Incrementa tamanho do buffer
                        }
                        else
                        {
                            //Aqui estourou o buffer. Reiniciar máqina de estados
                            //Descartar todos os dados recebidos
                            sCOM2RxBuffer.Status = SLOWCOMM_IDLE;
                            sCOM2RxBuffer.Pointer = 0;
                            sCOM2RxBuffer.Size = 0;
                        }
                    }
                }
                else
                {
                    //Aqui foi recebido um ETX, finalizando a mensagem
                    //Alterar estado para SLOWCOMM_BUSY
                    sCOM2RxBuffer.Status = SLOWCOMM_JOB;
                    sCOM2RxBuffer.Pointer = 0;
                    sCOM2RxBuffer.Size++;           //Incrementa tamanho do buffer
                    ucUartSel = COM2;
                }
                break;
            }
            //
            case SLOWCOMM_BUSY:
            {
                //Aqui o estado é BUSY. Não fazer nada.
                //O canal serial ficará inoperante até o comando ser processado.
                break;
            }
        }
    }
    else
    {
        //No modo transparente
        sCOM2RxBuffer.Data[sCOM2RxBuffer.Pointer] = U2RXREG;
        sCOM2RxBuffer.Pointer++;
        sCOM2RxBuffer.Size++;
        if(sCOM2RxBuffer.Size > RX_BUFFER_SIZE)
        {
            sCOM2RxBuffer.Size = 0;
            sCOM2RxBuffer.Pointer = 0;
        }
        
        //U1TXREG = sCOM2RxBuffer.Data[sCOM2RxBuffer.Pointer];
        
        //U1TXREG = U2RXREG;
        
    }
    IFS1bits.U2RXIF = 0;            //Zera flag de interrupção
}
//
void __attribute__((__interrupt__,no_auto_psv)) _U2TXInterrupt(void)
{
    if(sDevice.BlueToothTrasparent == BT_NORMAL_MODE)
    {
        sCOM2TxBuffer.Pointer++;            //Aponta para o próximo byte
        if(sCOM2TxBuffer.Pointer < (sCOM2TxBuffer.Size))
        {
            //Ainda há bytes para serem enviados
            U2TXREG = sCOM2TxBuffer.Data[sCOM2TxBuffer.Pointer];         //Envia byte para a UART
        }
        else
        {
            //Todos os bytes foram enviados
            //Libera buffer TX
            sCOM2TxBuffer.Status = SLOWCOMM_TX_EMPTY;
            sCOM2TxBuffer.Pointer = 0;
            sCOM2TxBuffer.Size = 0;
        }
    }
    else
    {
        //No modo transparente
        
    }
    IFS1bits.U2TXIF = 0;            //Zera flag de interrupção
}
//
//
void __attribute__((__interrupt__,no_auto_psv)) _U1RXInterrupt(void)
{
#if HC06_CONFIG == 1
    sCOM1RxBuffer.Data[sCOM1RxBuffer.Pointer] = U1RXREG;            //Obtém byte recebido pela UART
    sCOM1RxBuffer.Pointer++;
    IFS0bits.U1RXIF = 0;            //Zera flag de interrupção
    return;
#endif    
    if(sDevice.BlueToothTrasparent == BT_NORMAL_MODE)
    {
        //
//        #if(HC06_CONN == 2)
//        //Recepção serial do módulo WiFi
//        //Verifica se há espaço no buffer
//        if(sCOM1RxBuffer.Pointer < RX_BUFFER_SIZE)
//        {
//            sCOM1RxBuffer.Data[sCOM1RxBuffer.Pointer] = U1RXREG;    //Obtém byte recebido pela UART
//            sCOM1RxBuffer.Pointer++;                                //Incrementa ponteiro
//            sCOM1RxBuffer.Size++;                                   //Incrementa tamanho do buffer           
//        }
//        else
//        {
//            //Não há espaço no buffer
//            //Apontar para o início do buffer
//            sCOM1RxBuffer.Data[0] = U1RXREG;                        //Obtém byte recebido pela UART
//            sCOM1RxBuffer.Pointer = 1;                              //Incrementa ponteiro
//            sCOM1RxBuffer.Size = 1;                                 //Incrementa tamanho do buffer   
//        }
//        #else   
        sCOM1RxBuffer.Data[sCOM1RxBuffer.Pointer] = U1RXREG;            //Obtém byte recebido pela UART
        switch(sCOM1RxBuffer.Status)
        {
            case SLOWCOMM_IDLE:
            {
                //Aqui está esperando por um STX para iniciar o armazenamento no buffer
                if(sCOM1RxBuffer.Data[0] == STX)
                {
                    //Aqui foi recebido um STX
                    //Apontar para o início do buffer de armazenamento
                    //Mudar para estado SLOWCOMM_STX_RECEIVED
                    sCOM1RxBuffer.Pointer = 1;        //Aponta para o primeiro byte de dados no buffer
                    sCOM1RxBuffer.Status = SLOWCOMM_STX_RECEIVED;
                    sCOM1RxBuffer.Size = 1;
                 }
                break;
            }
            //
            case SLOWCOMM_STX_RECEIVED:
            {
                if(sCOM1RxBuffer.Data[sCOM1RxBuffer.Pointer] != ETX)
                {
                    if(sCOM1RxBuffer.Data[sCOM1RxBuffer.Pointer] == STX)
                    {
                        //Aqui foi recebido um STX. Fazer Resync.
                        sCOM1RxBuffer.Data[0] = sCOM1RxBuffer.Data[sCOM1RxBuffer.Pointer];
                        sCOM1RxBuffer.Pointer = 1;
                        sCOM1RxBuffer.Size = 1;
                    }
                    else
                    {
                        //Aqui foi recebido um dado válido
                        //Verificar se há espaço disponível no buffer
                        if(sCOM1RxBuffer.Pointer < SLOW_RX_MAX_BUF_SIZE)
                        {
                            //Ainda há espaço no buffer
                            //Armazenar dado
                            sCOM1RxBuffer.Pointer++;        //Incrementa ponteiro
                            sCOM1RxBuffer.Size++;           //Incrementa tamanho do buffer
                        }
                        else
                        {
                            //Aqui estourou o buffer. Reiniciar máqina de estados
                            //Descartar todos os dados recebidos
                            sCOM1RxBuffer.Status = SLOWCOMM_IDLE;
                            sCOM1RxBuffer.Pointer = 0;
                            sCOM1RxBuffer.Size = 0;
                        }
                    }
                }
                else
                {
                    //Aqui foi recebido um ETX, finalizando a mensagem
                    //Alterar estado para SLOWCOMM_BUSY
                    sCOM1RxBuffer.Status = SLOWCOMM_JOB;
                    sCOM1RxBuffer.Pointer = 0;
                    sCOM1RxBuffer.Size++;           //Incrementa tamanho do buffer
                    ucUartSel = COM1;
                }
                break;
            }
            //
            case SLOWCOMM_BUSY:
            {
                //Aqui o estado é BUSY. Não fazer nada.
                //O canal serial ficará inoperante até o comando ser processado.
                break;
            }
        }
        //
        //#endif
    }
    else
    {
        //Em modo transparente
        //Testar "Escape Message" = +-#?
        ucTransparentRxData = U1RXREG;
        U2TXREG = ucTransparentRxData;
        switch(ucTransparentRxCounter)
        {
            case 0:
            {
                if(ucTransparentRxData == '+')
                    ucTransparentRxCounter++;
                break;
            }
            case 1:
            {
                if(ucTransparentRxData == '-')
                    ucTransparentRxCounter++;
                else
                    ucTransparentRxCounter = 0;
                break;
            }
            case 2:
            {
                if(ucTransparentRxData == '#')
                    ucTransparentRxCounter++;
                else
                    ucTransparentRxCounter = 0;
                break;
            }     
            case 3:
            {
                if(ucTransparentRxData == '?')
                {
                    //Recebido mensagem de "Escape"
                    //Sair do modo transparente
                    ucTransparentEscape = 1;
                }
                else
                    ucTransparentRxCounter = 0;
                break;
            }
            default: ucTransparentRxCounter = 0; break;
        }
        
    }
    IFS0bits.U1RXIF = 0;            //Zera flag de interrupção
}
//
void __attribute__((__interrupt__,no_auto_psv)) _U1TXInterrupt(void)
{
    if(sDevice.BlueToothTrasparent == BT_NORMAL_MODE)
    {
        sCOM1TxBuffer.Pointer++;            //Aponta para o próximo byte
        if(sCOM1TxBuffer.Pointer < (sCOM1TxBuffer.Size))
        {
            //Ainda há bytes para serem enviados
            U1TXREG = sCOM1TxBuffer.Data[sCOM1TxBuffer.Pointer];         //Envia byte para a UART
        }
        else
        {
            //Todos os bytes foram enviados
            //Libera buffer TX
            sCOM1TxBuffer.Status = SLOWCOMM_TX_EMPTY;
            sCOM1TxBuffer.Pointer = 0;
            sCOM1TxBuffer.Size = 0;
            #if(PRODUCT_TYPE == 0)
                RTS_EXP_PIN = OFF;
            #endif
        }
    }
    else
    {
        //No modo transparente
        sCOM1TxBuffer.Pointer++;            //Aponta para o próximo byte
        if(sCOM1TxBuffer.Pointer < (sCOM1TxBuffer.Size))
        {
            //Ainda há bytes para serem enviados
            U1TXREG = sCOM1TxBuffer.Data[sCOM1TxBuffer.Pointer];         //Envia byte para a UART
        }
        else
        {
            //Todos os bytes foram enviados
            //Libera buffer TX
            sCOM1TxBuffer.Status = SLOWCOMM_TX_EMPTY;
            sCOM1TxBuffer.Pointer = 0;
            sCOM1TxBuffer.Size = 0;
            #if(PRODUCT_TYPE == 0)
                RTS_EXP_PIN = OFF;
            #endif
        }        
        
    }
    IFS0bits.U1TXIF = 0;            //Zera flag de interrupção
}
//
void COM2TxStart(void)
{
    //Inicia a transmissão pela UART do conteúdo do buffer de TX
    //O primeiro byte é enviado para a UART. Os demais bytes são tratados
    //automaticamente pelo serviço de interrupção até o último byte
    //Caso haja uma transmissão em curso, esta função não tem efeito
    //
    if(sCOM2TxBuffer.Status == SLOWCOMM_TX_EMPTY)
    {
        sCOM2TxBuffer.Status = SLOWCOMM_TX_BUSY;        //Muda estado
        sCOM2TxBuffer.Pointer = 0;                      //Aponta para o primeiro byte do buffer
        U2TXREG = sCOM2TxBuffer.Data[sCOM2TxBuffer.Pointer];//Envia primeiro byte para a UART
    }
}
//
void COM1TxStart(void)
{
    //Inicia a transmissão do primeiro dado contido no buffer Tx.
    //O demais dados são gerenciados pela interrupção da Uart.
    //sCOM1TxBuffer.Status
    if(sCOM1TxBuffer.Status == SLOWCOMM_TX_EMPTY)
    {
        sCOM1TxBuffer.Status = SLOWCOMM_TX_BUSY;        //Muda estado
        sCOM1TxBuffer.Pointer = 0;                      //Aponta para o primeiro byte do buffer
        U1TXREG = sCOM1TxBuffer.Data[sCOM1TxBuffer.Pointer];//Envia primeiro byte para a UART
    }    
}
//

