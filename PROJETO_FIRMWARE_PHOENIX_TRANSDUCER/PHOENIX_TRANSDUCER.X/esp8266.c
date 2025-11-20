/*
#include <stdio.h>
#include <stdlib.h>
#include <p24EP512GP806.h>
#include "hardware.h"
#include "io.h"
#include "ad.h"
#include "uart.h"
#include "timer.h"
#include "esp8266.h"
#include "string.h"
//
ESP8266Struct sEsp8266;
char EspTempBuffer[16];
unsigned char ucEspTemp;
unsigned int uiEspTemp;

extern unsigned char ucUartSel;

#define     ESP_IDLE            0
#define     ESP_RECEIVING       1
#define     ESP_PENDING         2

void RxCmdValidation(void);

void ESP8266UartRxHandler(void);
void ESP8266TxHandler(void);
void ESP8266StateHandler(void);
void ESP8266SendCommand(unsigned int ucDataSize);
void ESP8266UartDataHandler(void);
void ESP8266RxBufferReset(void);

void ESP8266LoadParams(void)
{
	sEsp8266.State = ESP_STATE_POWERON;
    sEsp8266.SubState = 0;
	sEsp8266.Station.DefaultMode = ESP_STATION_MODE - 0x30;
	sEsp8266.Station.Dhcp = TRUE;
	sEsp8266.Station.Waitflag = FALSE;
    //
	strcpy(sEsp8266.Station.Ip, ESP_DEFAULT_STATION_IP);
	strcpy(sEsp8266.Station.Gateway, ESP_DEFAULT_STATION_GATEWAY);
	strcpy(sEsp8266.Station.SubnetMask, ESP_DEFAULT_STATION_SUBNETMASK);
	strcpy(sEsp8266.Station.Ssid, ESP_DEFAULT_STATION_SSID );
	strcpy(sEsp8266.Station.Password, ESP_DEFAULT_STATION_PASSWORD);
    //
    strcpy(sEsp8266.AccessPoint.Ssid, ESP_DEFAULT_AP_SSID);
    strcpy(sEsp8266.AccessPoint.Password, ESP_DEFAULT_AP_PASSWORD);
    strcpy(sEsp8266.AccessPoint.Ip, ESP_DEFAULT_AP_IP);
    strcpy(sEsp8266.AccessPoint.Gateway, ESP_DEFAULT_AP_GATEWAY);
    strcpy(sEsp8266.AccessPoint.SubnetMask, ESP_DEFAULT_AP_SUBNETMASK);
    //
    sEsp8266.ReceivedCmd.AtOk = 0;
    sEsp8266.ReceivedCmd.Unknow = 0;
    sEsp8266.ReceivedCmd.Valid = 0;
    sEsp8266.ReceivedCmd.Error = 0;
    sEsp8266.TxStatus = ESP_TX_IDLE;
    sEsp8266.RxUartTimeout = ESP_UART_RX_TIMEOUT;
    sEsp8266.TcpTxStart = 0;
    sEsp8266.TcpTxState = 0;
    
    TimerStop(ESP_LEDTIMER);
    sEsp8266.StatusLed.BlinkTurns = 1;
    sEsp8266.StatusLed.TurnsCounter = 0;
    sEsp8266.StatusLed.Enable = DISABLE;
    LED_GREEN_PIN = _OFF;
    LED_YELLOW_PIN = _OFF;
    sEsp8266.StatusLed.State = T_OFF;
}
//
void ESP8266Init(void)
{
    BLUETOOTH_PWR_ENABLE_PIN = ESP8266_RESET_ENABLE;      //Habilita Reset
    TimerLoad(ESP_TIMER, ESP_RESET_TIMEOUT, RUN);
    if(TimerStatus(ESP_TIMER) == OVERFLOW)
    {
        TimerStop(ESP_TIMER);
        BLUETOOTH_PWR_ENABLE_PIN = ESP8266_RESET_DISABLE;      //Desabilita Reset
    }
//    
//    ESP8266LoadParams();
//    BLUETOOTH_PWR_ENABLE_PIN = ESP8266_RESET_DISABLE;
}
//
void ESP8266ResetHandler(void)
{
    if(sEsp8266.SubState == 0)
    {
        BLUETOOTH_PWR_ENABLE_PIN = ESP8266_RESET_ENABLE;      //Habilita Reset
        TimerLoad(ESP_TIMER, ESP_RESET_TIMEOUT, RUN);
        sEsp8266.SubState = 1;
    }
    else if(sEsp8266.SubState == 1)
    {
        if(TimerStatus(ESP_TIMER) == OVERFLOW)
        {
            TimerStop(ESP_TIMER);
            sEsp8266.SubState = 2;
            TimerLoad(ESP_TIMER, ESP_RESET_TIMEOUT*3, RUN);
            BLUETOOTH_PWR_ENABLE_PIN = ESP8266_RESET_DISABLE;      //Desabilita Reset
        }
    }
    else if(sEsp8266.SubState == 2)
    {
        TimerStop(ESP_TIMER);
        sEsp8266.SubState = 3;
    }
}
//
void ESP8266PowerOnHandler(void)
{
    //  subState:
    //  0 = Entrada
    //  1 = Aguardando timeout
    //  2 = Timeout atingido
    if(sEsp8266.SubState == 0)
    {
        TimerLoad(ESP_TIMER, ESP_POWERON_TIMEOUT, RUN);
        sEsp8266.SubState = 1;
    }
    else if(sEsp8266.SubState == 1)
    {
        if(TimerStatus(ESP_TIMER) == OVERFLOW)
        {
            sEsp8266.SubState = 2;
            TimerStop(ESP_TIMER);
        }
    }
}
//
void ESP8266DetectHandler(void)
{
    if(sEsp8266.SubState == 0)
    {
        //Envia comando AT (verificar presença do Módulo ESP8266)
        strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_AT);
        ESP8266SendCommand(4);
        TimerLoad(ESP_TIMER, ESP_DETECT_TIMEOUT, RUN);  
        sEsp8266.SubState = 1;
    }
    else if(sEsp8266.SubState == 1)
    {
        if(TimerStatus(ESP_TIMER) == OVERFLOW)
        {
            sEsp8266.SubState = 2;
            TimerStop(ESP_TIMER);
        }
    }
}
//
void ESP8266StatusLedHandler(void)
{
    if(sEsp8266.StatusLed.Enable == BLINK)
    {
        if(sEsp8266.StatusLed.TurnsCounter < sEsp8266.StatusLed.BlinkTurns)
        {
            if(sEsp8266.StatusLed.State == T_OFF)
            {
                if(TimerStatus(ESP_LEDTIMER) == STOP)
                {
                    TimerLoad(ESP_LEDTIMER, 200, RUN);
                }
                if(TimerStatus(ESP_LEDTIMER) == OVERFLOW)
                {
                    TimerStop(ESP_LEDTIMER);
                    sEsp8266.StatusLed.State = T_ON;
                }
            }
            else
            {
                if(TimerStatus(ESP_LEDTIMER) == STOP)
                {
                    TimerLoad(ESP_LEDTIMER, 200, RUN);
                    LED_GREEN_PIN = _ON;

                }
                if(TimerStatus(ESP_LEDTIMER) == OVERFLOW)
                {
                    TimerStop(ESP_LEDTIMER);
                    LED_GREEN_PIN = _OFF;
                    sEsp8266.StatusLed.State = T_OFF;
                    sEsp8266.StatusLed.TurnsCounter++;
                }
            }
        }
        else
        {
            if(TimerStatus(ESP_LEDTIMER) == STOP)
            {
               TimerLoad(ESP_LEDTIMER, 1000, RUN);
            }  
            if(TimerStatus(ESP_LEDTIMER) == OVERFLOW)
            {
                TimerStop(ESP_LEDTIMER);
                sEsp8266.StatusLed.State = T_OFF;
                sEsp8266.StatusLed.TurnsCounter = 0;
            }
        }
    }
    else if(sEsp8266.StatusLed.Enable == ENABLE)
        LED_GREEN_PIN = _ON;
    else
        LED_GREEN_PIN = _OFF;
}
//
void ESP8266SetStatusLed(unsigned char State, unsigned char BlinkValue)
{
    TimerStop(ESP_LEDTIMER);
    sEsp8266.StatusLed.BlinkTurns = BlinkValue;
    sEsp8266.StatusLed.TurnsCounter = 0;
    sEsp8266.StatusLed.Enable = State;
    sEsp8266.StatusLed.State = T_OFF;   
}
//
void ESP8266WiFiConfigHandler(void)
{
    switch(sEsp8266.SubState)
    {
        case 0:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Envia comando AT+CWMODE_CUR?Sets the Current Wi-Fi mode;
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_WIFI_MODE);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=3\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.WifiMode == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.WifiMode = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE;
                    sEsp8266.SubState = 1;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            }
            ESP8266TxHandler();
            break;
        }
        //
        case 1:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Enivar comando AT+CWSAP_CUR
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_SET_SOFTAP);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.AccessPoint.Ssid);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\",\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.AccessPoint.Password);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\",5,3\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.SetAp == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.SetAp = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 2;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            } 
            ESP8266TxHandler();
            break;
        }
        //
        case 2:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Enivar comando AT+CWDHCP_CUR
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_DHCP);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=0,0\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_SHORT_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;                
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.SetDhcp == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.SetDhcp = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 3;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            } 
            ESP8266TxHandler();
            break;
        }
        //
        case 3:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Enivar comando AT+CIPAP_CUR
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_SET_SOFTAP_IP);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.AccessPoint.Ip);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\",\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.AccessPoint.Gateway);  
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\",\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.AccessPoint.SubnetMask);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\"\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_SHORT_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START; 
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.SetApIp == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.SetApIp = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 4;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            } 
            ESP8266TxHandler();
            break;
        }
        //
        case 4:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Enviar comando AT+CIPSTA_CUR
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_SET_STATION_IP);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.Station.Ip);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\",\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.Station.Gateway);  
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\",\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.Station.SubnetMask);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\"\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_SHORT_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;                 
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.SetStationIp == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.SetStationIp = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 5;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            } 
            ESP8266TxHandler();            
            break;
        }
        //
        case 5:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Configura o modo de transmissão
                //CIPMODE = 0
                //Envia comando AT+CIPMODE
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_TRANSMISSION_MODE);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=0\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_SHORT_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;                 
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.CipMode == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.CipMode = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 6;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            } 
            ESP8266TxHandler();            
            break;
        }
        //
        case 6:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Habilita múltiplas conexões
                //CIPMUX = 1
                //Envia comando AT+CIPMUX
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_MULT_CONNECTION_MODE);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=1\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_SHORT_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;                 
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.CipMux == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.CipMux = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 7;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            } 
            ESP8266TxHandler();             
            break;
        }
        //
        case 7:
        {
            
            break;
        }
    }
}
//
void ESP8266ConnectApHandler(void)
{
    switch(sEsp8266.SubState)
    {
        case 0:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Desconecta de qualquer AP
                //Enviar comando AT+CWQAP
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_DISCONNECT_AP);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_SHORT_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;                  
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.DisconnectAp == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.DisconnectAp = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 1;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                Nop();
                Nop();
                Nop();
                Nop();
                sEsp8266.TxStatus = ESP_TX_IDLE;
            }            
            ESP8266TxHandler();
            break;
        }
        //
        case 1:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Conecta ao AP cadastrado
                //Envia comando AT+CWJAP_CUR
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_CONNECT_AP);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.Station.Ssid);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\",\"");
                strcat((char*)sCOM1TxBuffer.Data,(char*)sEsp8266.Station.Password);  
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\"\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_SHORT_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;                 
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.ConnectAp == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.ConnectAp = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 2;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            }            
            ESP8266TxHandler();            
            break;
        }
        //
        case 2:
        {
            //Aguarda resposta da conexão ao AP
            if(TimerStatus(ESP_TIMER) == STOP)
                TimerLoad(ESP_TIMER, ESP_CONNECT_AP_TIMEOUT, RUN);
            else if(TimerStatus(ESP_TIMER) == OVERFLOW)
            {
                //A resposta não chegou
                TimerStop(ESP_TIMER);
                sEsp8266.TxStatus = ESP_TX_IDLE;
                sEsp8266.SubState = 0;
            }
            if((sEsp8266.ReceivedCmd.WifiConnected == 1)) //&& (sEsp8266.ReceivedCmd.WifiGotIp == 1)
            {
                //Conexão com sucesso
                TimerStop(ESP_TIMER);
                sEsp8266.TxStatus = ESP_TX_IDLE;
                sEsp8266.ReceivedCmd.WifiConnected = 0;
                sEsp8266.ReceivedCmd.WifiGotIp = 0;
                sEsp8266.ReceivedCmd.Valid = 0;
                sEsp8266.SubState = 3;
            }
            break;
        }
        //
        case 3:
        {

            
            break;
        }
    }
}
//
void ESP8266TcpHandler(void)
{
    switch(sEsp8266.SubState)
    {
        case 0:
        {
            //Exclui server (mesmo que não exista)
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Desconecta de qualquer AP
                //Enviar comando AT+CIPSERVER
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_TCP_SERVER);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=0\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_SHORT_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;                  
            }            
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.CipServer == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.CipServer = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 1;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            }            
            ESP8266TxHandler(); 
            break;
        }
        //
        case 1:
        {
            //Inicia server
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Enviar comando AT+CIPSERVER
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_TCP_SERVER);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=1,");
                strcat((char*)sCOM1TxBuffer.Data,(char*)ESP_DEFAULT_TCP_PORT);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_TIMEOUT_CONFIG;
                sEsp8266.TxStatus = ESP_TX_START;                  
            } 
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.CipServer == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.CipServer = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.SubState = 2;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            }            
            ESP8266TxHandler();            
            break;
        }
        case 2:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)  
            {
                
                
            }
            break;
        }
    }
}
//
void ESP8266TcpSendData(void)
{
    switch(sEsp8266.TcpTxState)
    {
        case 0:
        {
            if(sEsp8266.TcpTxStart == 1)
            {
                sEsp8266.TcpTxStart = 0;
                sEsp8266.TcpTxState = 1;
                sEsp8266.TxStatus = ESP_TX_IDLE;
            }
            break;
        }
        //
        case 1:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Enviar comando AT+CIPSEND
                strcpy((char*)sCOM1TxBuffer.Data,(char*)ESP_SEND_DATA);
                strcat((char*)sCOM1TxBuffer.Data,(char*)"=");
                //itoa(EspTempBuffer, sEsp8266.CurrentSocketId, 10);
                strcat((char*)sCOM1TxBuffer.Data,(char*)EspTempBuffer);
                strcat((char*)sCOM1TxBuffer.Data,(char*)",");
                //itoa(EspTempBuffer, sEsp8266.TcpTxSize, 10);
                strcat((char*)sCOM1TxBuffer.Data,(char*)EspTempBuffer);                
                strcat((char*)sCOM1TxBuffer.Data,(char*)"\r\n");
                sEsp8266.RxUartTimeout = ESP_UART_RX_TIMEOUT;
                sEsp8266.TxStatus = ESP_TX_START;                   
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.TxPrompt == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.TxPrompt = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.TcpTxState = 2;
                }
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
            }            
            ESP8266TxHandler();              
            break;
        }
        //
        case 2:
        {
            if(sEsp8266.TxStatus == ESP_TX_IDLE)
            {
                //Enviar o buffer uiEspTemp
                for(uiEspTemp = 0; uiEspTemp < sEsp8266.TcpTxSize; uiEspTemp++)
                    sCOM1TxBuffer.Data[uiEspTemp] = sEsp8266.TcpTxBuffer[uiEspTemp];
                sCOM1TxBuffer.Data[uiEspTemp] = 0x00;
                sEsp8266.RxUartTimeout = ESP_UART_RX_TIMEOUT;
                sEsp8266.TxStatus = ESP_TX_START;                 
            }
            else if(sEsp8266.TxStatus == ESP_TX_COMPLETE)
            {
                //Foi recebido resposta válida
                //Verificar consistência
                if(sEsp8266.ReceivedCmd.TxSendOk == 1)
                {
                    //Resposta OK
                    TimerStop(ESP_TIMER);
                    sEsp8266.ReceivedCmd.TxSendOk = 0;
                    sEsp8266.TxStatus = ESP_TX_IDLE; 
                    sEsp8266.TcpTxState = 0;
                }                
            }
            else if((sEsp8266.TxStatus == ESP_TX_ERROR) || (sEsp8266.TxStatus == ESP_TX_INVALID) || (sEsp8266.TxStatus == ESP_TX_TIMEOUT))
            {
                //Ocorreu erro
                sEsp8266.TxStatus = ESP_TX_IDLE;
                sEsp8266.TcpTxState = 0;
            }             
            ESP8266TxHandler();
            break;
        }
    }
}
//
void ESP8266TxHandler(void)
{
    switch(sEsp8266.TxStatus)
    {
        case ESP_TX_IDLE:
        {
            
            break;
        }
        //
        case ESP_TX_START:
        {
            //Inicia processo de envio dos dados
            TimerStop(ESP_TIMER);
            sEsp8266.ReceivedCmd.Valid = 0;
            sEsp8266.ReceivedCmd.Error = 0;
            sEsp8266.ReceivedCmd.Unknow = 0;
            ESP8266SendCommand((unsigned int)(strlen((char*)sCOM1TxBuffer.Data)));
            TimerLoad(ESP_TIMER, ESP_COMMAND_TIMEOUT, RUN);
            sEsp8266.TxStatus = ESP_TX_WAIT;
            break;
        }
        //
        case ESP_TX_WAIT:
        {
            //Aguarda resposta do módulo
            if(sEsp8266.ReceivedCmd.Valid == 1)
            {
                //Foi recebido uma resposta de comando válido
                //Limpa flag de comando válido
                sEsp8266.ReceivedCmd.Valid = 0;
                //para Timer
                TimerStop(ESP_TIMER);
                //Alterna estado
                sEsp8266.TxStatus = ESP_TX_COMPLETE;
            }
            else if(sEsp8266.ReceivedCmd.Error == 1)
            {
                //Foi recebida uma resposta de ERROR
                //Limpa Flag de Erro
                sEsp8266.ReceivedCmd.Error = 0;
                //Para Timer
                TimerStop(ESP_TIMER);
                //Alterna estado
                sEsp8266.TxStatus = ESP_TX_ERROR;
            }
            else if(sEsp8266.ReceivedCmd.Unknow == 1)
            {
                //Foi recebida uma resposta de comando inválido
                //Limpa Flag de comando inválido
                sEsp8266.ReceivedCmd.Unknow = 0;
                //Para Timer
                TimerStop(ESP_TIMER);
                //Alterna estado
                sEsp8266.TxStatus = ESP_TX_INVALID;
            }    
            else if(TimerStatus(ESP_TIMER) == OVERFLOW)
            {
                //O módulo não respondeu
                //Ocorreu time-out
                //Para Timer
                TimerStop(ESP_TIMER);
                //Alterna estado
                sEsp8266.TxStatus = ESP_TX_TIMEOUT;
            }
            break;
        }
        //
        case ESP_TX_COMPLETE:
        {
            
            break;
        }
        //
        case ESP_TX_ERROR:
        {
            
            break;
        }
        //
        case ESP_TX_INVALID:
        {
            
            break;
        }
        //
        case ESP_TX_TIMEOUT:
        {
            
            break;
        }
        //
        default: break;
    }
}
//
void ESP8266StateHandler(void)
{
    ESP8266UartRxHandler();
    ESP8266StatusLedHandler();
    if(sCOM1RxBuffer.Status == ESP_PENDING)
        ESP8266UartDataHandler();
    switch(sEsp8266.State)
    {
        case ESP_STATE_POWERON:
		{
            //Verifica estado do power on do módulo
            ESP8266PowerOnHandler();
            if(sEsp8266.SubState == 2)
            {
                sEsp8266.State = ESP_STATE_RESETING;
                sEsp8266.SubState = 0;
                TimerStop(ESP_TIMER);
            }
            break;
        }
        //
        case ESP_STATE_RESETING:
        {
            ESP8266ResetHandler();
            if(sEsp8266.SubState == 3)
            {
                sEsp8266.SubState = 0;
                sEsp8266.State = ESP_STATE_DETECTING;
                TimerStop(ESP_TIMER);
                ESP8266SetStatusLed(BLINK,4);
            }
            break;
        }
        //
        case ESP_STATE_DETECTING:
        {
            //Verifica se recebeu resposta ao comando AT
//            ESP8266DetectHandler();
//            if(sEsp8266.SubState == 1 && sEsp8266.ReceivedCmd.AtOk == 1)
//            {
//                //Recebido comando resposta ao AT
//                //Módulo detectado
//                sEsp8266.SubState = 0;
//                sEsp8266.State = ESP_STATE_CONFIG_WIFI;
//                sEsp8266.ReceivedCmd.AtOk = 0;
//                TimerStop(ESP_TIMER);
//                ESP8266SetStatusLed(BLINK,2);
//            }
//            else if(sEsp8266.SubState == 1 && sEsp8266.ReceivedCmd.Unknow == 1)
//            {
//                //Recebido resposta qualquer/estranha
//                TimerStop(ESP_TIMER);
//                sEsp8266.ReceivedCmd.Unknow = 0;
//                sEsp8266.SubState = 0;
//            }
//            else if(sEsp8266.SubState == 2)
//            {
//                //Ocorreu Timeout
//                sEsp8266.SubState = 0;
//                sEsp8266.State = ESP_STATE_POWERON;
//            }
            break;
        }
        //
        case ESP_STATE_CONFIG_WIFI:
        {
            //Configura o módulo
            ESP8266WiFiConfigHandler();
            if(sEsp8266.SubState == 7)
            {
                ESP8266SetStatusLed(BLINK,3);
                sEsp8266.State = ESP_STATE_CONNECT_AP;
                sEsp8266.SubState = 0; 
            }
            break;
        }
        //
        case ESP_STATE_CONNECT_AP:
        {
            ESP8266ConnectApHandler();
            if(sEsp8266.SubState == 3)
            {
                ESP8266SetStatusLed(BLINK,4);
                sEsp8266.State = ESP_STATE_WAIT_TCP;
                sEsp8266.SubState = 0;
            }
            break;
        }
        //
        case ESP_STATE_IDLE:
        {
            
            break;
        }
        //
        case ESP_STATE_WAIT_TCP:
        {
            ESP8266TcpHandler();
            if(sEsp8266.SubState == 2)
            {
                ESP8266SetStatusLed(ENABLE,0);
                sEsp8266.State = ESP_STATE_IDLE_TCP;
                sEsp8266.SubState = 0;
            }
            break;
        }
        //
        case ESP_STATE_IDLE_TCP:
        {
            if(sEsp8266.ReceivedCmd.TcpReceive == 1)
            {
                sEsp8266.ReceivedCmd.TcpReceive = 0;
                sEsp8266.ReceivedCmd.Valid = 0;
                if(sEsp8266.Tcp[0].RxSize > 0)
                {
                    sEsp8266.CurrentSocketId = 0;
                    ucUartSel = ETH;
                    RxCmdValidation();
                }
                if(sEsp8266.Tcp[1].RxSize > 0)
                {
                    sEsp8266.CurrentSocketId = 1;
                    ucUartSel = ETH;
                    RxCmdValidation();
                }
                if(sEsp8266.Tcp[2].RxSize > 0)
                {
                    sEsp8266.CurrentSocketId = 2;
                    ucUartSel = ETH;
                    RxCmdValidation();
                }
                if(sEsp8266.Tcp[3].RxSize > 0)
                {
                    sEsp8266.CurrentSocketId = 3;
                    ucUartSel = ETH;
                    RxCmdValidation();
                } 
            }
            ESP8266TcpSendData();
            break;
        }
        //
        case ESP_STATE_BUSY:
        {
            Nop();
            Nop();
            break;
        }
    }
}
//
void ESP8266UartRxHandler(void)
{
//    //Verifica se há bytes recebidos no buffer
//    if(sCOM1RxBuffer.Size > 6)
//    {
//        //Verifica se houve alteração na quantidade de bytes recebidos
//        if(sCOM1RxBuffer.Size != sCOM1RxBuffer.MessageSize)
//        {
//            //A quantidade é diferente
//            sCOM1RxBuffer.MessageSize = sCOM1RxBuffer.Size;
//            //Para Timer
//            TimerStop(ESP_UART_TIMER);
//            sCOM1RxBuffer.Status = ESP_RECEIVING;
//        }
//        else
//        {
//            //As quantidade de bytes são iguais
//            //Disparar timer
//            if(TimerStatus(ESP_UART_TIMER) == STOP)
//            {
//                if((sEsp8266.RxUartTimeout >= ESP_UART_RX_TIMEOUT_MIN) && (sEsp8266.RxUartTimeout <= ESP_UART_RX_TIMEOUT_MAX))
//                    TimerLoad(ESP_UART_TIMER, sEsp8266.RxUartTimeout, RUN);
//                else
//                    TimerLoad(ESP_UART_TIMER, ESP_UART_RX_TIMEOUT, RUN);
//            }
//        }
//        if(TimerStatus(ESP_UART_TIMER) == OVERFLOW)
//        {
//            //Ocorreu Overflow
//            //Não foram recebidos mais bytes
//            //Notificar que há job pendente na serial    
//            //inclui terminador de string NULL
//            sCOM1RxBuffer.Data[sCOM1RxBuffer.Pointer] = 0x00;
//            //Para Timer
//            TimerStop(ESP_UART_TIMER);
//            //Notificar que há comando pendente na UART
//            sCOM1RxBuffer.Status = ESP_PENDING;
//        }
//    }
//    else
//    {
//        sCOM1RxBuffer.Status = ESP_IDLE;
//    }
}
//
void ESP8266UartDataHandler(void)
{
    unsigned int i;
    char *pTemp;
    sEsp8266.pRxBuffer = strstr((const char*)sCOM1RxBuffer.Data, ESP_TCP_RECEIVE_DATA);
    if(sEsp8266.pRxBuffer)
    {
        //Foram recebidos dados pela porta TCP
        //Obtém o ID da conexão
        ucEspTemp = (*(sEsp8266.pRxBuffer + 5)) - 0x30;
        //Verifica se está no intervalo permitido (0, 1, 2 ou 3)
        if(ucEspTemp >=0 && ucEspTemp <=3)
        {
            i = 0;
            //Aponta para o início do campo Size
            pTemp = sEsp8266.pRxBuffer + 7;
            //Extrai Size (valor ascii decimal entre os caracteres "," e ":")
            while(*pTemp != ':')
            {
                EspTempBuffer[i++] = *pTemp++;
                if(i>6)
                    break;
            }
            //Insere um final de string
            EspTempBuffer[i] = 0;
            pTemp++;
            //Converte o Size de Ascii para int
            sEsp8266.Tcp[ucEspTemp].RxSize = strtol(EspTempBuffer, 0x00, 10);
            //Move o conteúdo do buffer serial para o buffer TCP do ID correspondente
            for(i=0; i<sEsp8266.Tcp[ucEspTemp].RxSize; i++)
               sEsp8266.Tcp[ucEspTemp].RxBuffer[i] =  (*pTemp++);
            //Insere um final de string
            sEsp8266.Tcp[ucEspTemp].RxBuffer[i] = 0x00;
            //Habilita flags
            sEsp8266.ReceivedCmd.TcpReceive = 1;
            sEsp8266.ReceivedCmd.Valid = 1;
            
        }
    } 
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_TX_CIPSEND_RESPONSE))
    {
        if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_OK))
        {
            if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_OK))
            {
                //> Prompt de Tx recebido
                sEsp8266.ReceivedCmd.TxPrompt = 1;
                sEsp8266.ReceivedCmd.Valid = 1;
            }
        }
        else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_ERROR))
            sEsp8266.ReceivedCmd.Error = 1;        
    }    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_TX_SEND_OK_RESPONSE))
    {
        //> Prompt de Tx recebido
        sEsp8266.ReceivedCmd.TxSendOk = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    } 
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_AT_OK))
    {
        //Flag de AT OK recebido
        sEsp8266.ReceivedCmd.AtOk = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_CONNECT_AP_RESPONSE))
    {
        sEsp8266.ReceivedCmd.ConnectAp = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_TCP_CONNECT_MSG))
    {
        sEsp8266.ReceivedCmd.TcpConnect = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }
    // 
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_TCP_CLOSE_MSG))
    {
        sEsp8266.ReceivedCmd.TcpClose = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }
    //    
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_MODE_RESPONSE))
    {
        sEsp8266.ReceivedCmd.WifiMode = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_CONNECT_RESPONSE))
    {
        sEsp8266.ReceivedCmd.WifiConnected = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    } 
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_DISCONNECT_RESPONSE))
    {
        sEsp8266.ReceivedCmd.WifiDisconnected = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    } 
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_GOT_IP_RESPONSE))
    {
        sEsp8266.ReceivedCmd.WifiGotIp = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }     
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_ESP_SET_SOFTAP_RESPONSE))
    {
        if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_OK))
        {
            sEsp8266.ReceivedCmd.SetAp = 1;
            sEsp8266.ReceivedCmd.Valid = 1;
        }
        else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_ERROR))
            sEsp8266.ReceivedCmd.Error = 1;
    }
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_SET_DHCP_RESPONSE))
    {
        sEsp8266.ReceivedCmd.SetDhcp = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_SET_AP_IP_RESPONSE))
    {
        if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_OK))
        {
            sEsp8266.ReceivedCmd.SetApIp = 1;
            sEsp8266.ReceivedCmd.Valid = 1;
        }
        else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_ERROR))
            sEsp8266.ReceivedCmd.Error = 1;
    }
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_SET_STATION_IP_RESPONSE))
    {
        if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_OK))
        {
            sEsp8266.ReceivedCmd.SetStationIp = 1;
            sEsp8266.ReceivedCmd.Valid = 1;
        }
        else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_ERROR))
            sEsp8266.ReceivedCmd.Error = 1;
    } 
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_DISCONNECT_AP_RESPONSE))
    {
        sEsp8266.ReceivedCmd.DisconnectAp = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_TRANSFER_MODE_RESPONSE))
    {
        sEsp8266.ReceivedCmd.CipMode = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }    
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_MULT_CONNECTION_MODE_RESPONSE))
    {
        sEsp8266.ReceivedCmd.CipMux = 1;
        sEsp8266.ReceivedCmd.Valid = 1;
    }  
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_DELETE_SERVER_RESPONSE))
    {
        if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_OK))
        {
            sEsp8266.ReceivedCmd.CipServer = 1;
            sEsp8266.ReceivedCmd.Valid = 1;
        }
        else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_ERROR))
            sEsp8266.ReceivedCmd.Error = 1;        
    }    
    //
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_WIFI_CREATE_SERVER_RESPONSE))
    {
        if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_OK))
        {
            sEsp8266.ReceivedCmd.CipServer = 1;
            sEsp8266.ReceivedCmd.Valid = 1;
        }
        else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_ERROR))
            sEsp8266.ReceivedCmd.Error = 1;        
    }    
    //    
    else if(strstr((const char*)sCOM1RxBuffer.Data, ESP_RESPONSE_ERROR))
    {
        //Resposta: ERROR
        sEsp8266.ReceivedCmd.Error = 1;
    }
    //
    else
    {
        sEsp8266.ReceivedCmd.Unknow = 1;
    }
    ESP8266RxBufferReset();
}
//
void ESP8266SendCommand(unsigned int ucDataSize)
{
    //unsigned int i;
    //for(i=0;i>ucDataSize;i++)
    //    sCOM1TxBuffer.Data[i] = ucData[i];
    sCOM1TxBuffer.Pointer = 0;                      //Aponta para o primeiro byte a ser transmitido
    sCOM1TxBuffer.Size = ucDataSize;             //Determina o tamanho do buffer
    COM1TxStart();                                  //Dispara a transmissï¿½o pela UART
}
//
void ESP8266RxBufferReset(void)
{
    unsigned int j;
    sCOM1RxBuffer.Size = 0;
    sCOM1RxBuffer.Pointer = 0;
    sCOM1RxBuffer.MessageSize = 0;
    for(j=0; j<RX_BUFFER_SIZE;j++)
        sCOM1RxBuffer.Data[j] = 0;
}
*/