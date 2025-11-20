#ifndef ESP8266_H_
#define ESP8266_H_

#define ESP_COMM_RETRIES                    3
#define ESP_UART_RX_TIMEOUT                 10
#define ESP_UART_RX_TIMEOUT_MIN             5
#define ESP_UART_RX_TIMEOUT_MAX             1000
#define ESP_UART_RX_TIMEOUT_CONFIG          1000
#define ESP_UART_RX_SHORT_TIMEOUT_CONFIG    250
#define ESP_POWERON_TIMEOUT                 300
#define ESP_RESET_TIMEOUT                   100
#define ESP_DETECT_TIMEOUT                  1000
#define ESP_COMMAND_TIMEOUT                 ESP_UART_RX_TIMEOUT_CONFIG + 250
#define ESP_CONNECT_AP_TIMEOUT              5000


#define ESP8266_NORMAL_MODE         0
#define ESP8266_PROG_MODE           1

#define ESP8266_RESET_ENABLE        1
#define ESP8266_RESET_DISABLE       0


#define ESP_CR_LF						"\r\n"
#define ESP_QUESTION_MARK				"?"
#define ESP_QUERY_MARK					":"
#define ESP_SET_MARK					"="
#define ESP_SEPARATOR_MARK				","
#define ESP_QUOTE_MARK					"\""
#define ESP_ESCAPE_SEQ					"+++"
#define ESP_STATION_MODE				0x31
#define ESP_SOFTAP_MODE					0x32
#define ESP_SOFTAP_STATION_MODE			0x33
#define ESP_COMM_FAILED					1
#define ESP_STATION_DHCP_ENABLED		"1,1"
//
#define ESP_STATION_DHCP_DISABLED		"1,0"
#define ESP_DEFAULT_STATION_IP			"192.168.1.102"
#define ESP_DEFAULT_STATION_GATEWAY		"192.168.1.254"
#define ESP_DEFAULT_STATION_SUBNETMASK  "255.255.255.0"
#define ESP_DEFAULT_STATION_SSID        "PHOENIX_TRANSDUCER_AP"
#define ESP_DEFAULT_STATION_PASSWORD    "MSH31310"
#define ESP_DEFAULT_TCP_PORT			"4545"
//
//AP
//
#define ESP_DEFAULT_AP_SSID				"PHOENIX-WIFI-TEST"
#define ESP_DEFAULT_AP_PASSWORD			"12345678"
#define ESP_DEFAULT_AP_IP				"192.168.2.1"
#define ESP_DEFAULT_AP_GATEWAY			"192.168.2.1"
#define ESP_DEFAULT_AP_SUBNETMASK		"255.255.255.0"
//
//
//Comandos AT Básicos
//
#define ESP_AT							"AT\r\n"
#define ESP_AT_CMD_PREFIX				"AT+"
#define ESP_RESTART						"AT+RST"
#define ESP_INFO						"AT+GMR"
#define ESP_DEEP_SLEEP					"AT+GSLP"
#define ESP_ECHO						"ATE"
#define ESP_FACTORY_RESET				"AT+RESTORE"
#define ESP_UART_CONFIG					"AT+UART_CUR"
#define ESP_UART_DEFAULT				"AT+UART_DEF"
#define ESP_SLEEP						"AT+SLEEP"
#define ESP_RF_POWER					"AT+RFPOWER"
#define ESP_RF_POWER_VDD3				"AT+RFVDD"

//
//Comandos referentes ao WiFi
//
#define ESP_WIFI_MODE					"AT+CWMODE_CUR"
#define ESP_WIFI_MODE_FLASH				"AT+CWMODE_CUR_DEF"
#define ESP_CONNECT_AP					"AT+CWJAP_CUR"
#define ESP_CONNECT_AP_FLASH			"AT+CWJAP_DEF"
#define ESP_LIST_AP						"AT+CWLAP"
#define ESP_DISCONNECT_AP				"AT+CWQAP"
#define ESP_SET_SOFTAP					"AT+CWSAP_CUR"
#define ESP_SET_SOFTAP_FLASH			"AT+CWSAP_DEF"
#define ESP_GET_CONNECTED_AP			"AT+CWLIF"
#define ESP_DHCP						"AT+CWDHCP_CUR"
#define ESP_DHCP_FLASH					"AT+CWDHCP_DEF"
#define ESP_AUTOCONNECT_PWRON			"AT+CWAUTOCONN"
#define ESP_SET_STATION_MAC				"AT+CIPSTAMAC_CUR"
#define ESP_SET_STATION_MAC_FLASH		"AT+CIPSTAMAC_DEF"
#define ESP_SET_SOFTAP_MAC				"AT+CIPAPMAC_CUR"
#define ESP_SET_SOFTAP_MAC_FLASH		"AT+CIPAPMAC_DEF"
#define ESP_SET_STATION_IP				"AT+CIPSTA_CUR"
#define ESP_SET_STATION_FLASH			"AT+CIPSTA_DEF"
#define ESP_SET_SOFTAP_IP				"AT+CIPAP_CUR"
#define ESP_SET_SOFTAP_IP_FLASH			"AT+CIPAP_DEF"

//
//Comandos referentes ao TCP-IP
//
#define ESP_GET_CONNECTION_STATUS		"AT+CIPSTATUS"
#define ESP_START_TCP					"AT+CIPSTART"
#define ESP_SEND_DATA					"AT+CIPSEND"
#define ESP_SEND_DATA_EX				"AT+CIPSENDEX"
#define ESP_SEND_BUFFER					"AT+CIPSENDBUF"
#define ESP_BUFFER_RESET				"AT+CIPBUFRESET"
#define ESP_CHECK_BUFFER				"AT+CIPBUFSTATUS"
#define ESP_CHECK_SEGMENT				"AT+CIPCHECKSEQ"
#define ESP_CLOSE_TCP					"AT+CIPCLOSE"
#define ESP_GET_IP						"AT+CIFSR"
#define ESP_MULT_CONNECTION_MODE		"AT+CIPMUX"
#define ESP_TCP_SERVER					"AT+CIPSERVER"
#define ESP_TRANSMISSION_MODE			"AT+CIPMODE"
#define ESP_SAVE_TRANSPARENT			"AT+SAVETRANSLINK"
#define ESP_SET_TIMEOUT					"AT+CIPSTO"
#define ESP_FIRMWARE_UPDATE				"AT+CIUPDATE"
#define ESP_PING						"AT+PING"
#define ESP_SHOW_REMOTE_IP				"AT+CIPDINFO"
#define ESP_RECEIVE_NETWORK_DATA		"+IPD"

//
//Respostas
//
#define ESP_RESPONSE_AT_OK				"AT\r\r\n\r\nOK\r\n"
#define ESP_RESPONSE_OK					"OK\r\n"
#define ESP_RESPONSE_ERROR				"ERROR\r\n"



#define ESP_TCP_CONNECT_MSG                         "CONNECT\r\n"
#define ESP_TCP_CLOSE_MSG                           "CLOSED\r\n"
#define ESP_TCP_SENDOK_MSG                          "SEND OK\r\n"
#define ESP_TCP_RECEIVE_DATA                        "+IPD,"

#define ESP_WIFI_MODE_RESPONSE                      "AT+CWMODE_CUR=3\r\r\n\r\nOK\r\n"  
#define ESP_WIFI_TRANSFER_MODE_RESPONSE             "AT+CIPMODE=0\r\r\n\r\nOK\r\n"  
#define ESP_WIFI_MULT_CONNECTION_MODE_RESPONSE      "AT+CIPMUX=1\r\r\n\r\nOK\r\n"  
#define ESP_WIFI_ESP_SET_SOFTAP_RESPONSE            "AT+CWSAP_CUR=" 
#define ESP_WIFI_DELETE_SERVER_RESPONSE             "AT+CIPSERVER=0" 
#define ESP_WIFI_CREATE_SERVER_RESPONSE             "AT+CIPSERVER=1" 
#define ESP_WIFI_SET_DHCP_RESPONSE                  "AT+CWDHCP_CUR=0,0\r\r\n\r\nOK\r\n"   
#define ESP_WIFI_SET_AP_IP_RESPONSE                 "AT+CIPAP_CUR=" 
#define ESP_WIFI_SET_STATION_IP_RESPONSE            "AT+CIPSTA_CUR=" 
#define ESP_WIFI_DISCONNECT_AP_RESPONSE             "AT+CWQAP\r\r\n\r\nOK\r\n" 
#define ESP_WIFI_CONNECT_AP_RESPONSE                "AT+CWJAP_CUR=" 
#define ESP_WIFI_CONNECT_RESPONSE                   "WIFI CONNECTED\r\n"
#define ESP_WIFI_DISCONNECT_RESPONSE                "WIFI DISCONNECT\r\n"
#define ESP_WIFI_GOT_IP_RESPONSE                    "WIFI GOT IP\r\n"
#define ESP_TX_PROMPT_RESPONSE                      ">"
#define ESP_TX_SEND_OK_RESPONSE                     "SEND OK\r\n"
#define ESP_TX_CIPSEND_RESPONSE                     "AT+CIPSEND="
//
#define ESP_RECEIVE_STATUS_WAIT			0
#define ESP_RECEIVE_STATUS_TIMEOUT		1
#define ESP_RECEIVE_STATUS_OK			2
#define ESP_RECEIVE_STATUS_ERROR		3
//
enum STATION_STATE {WIFI_STATE_POWER_ON,
				 WIFI_STATE_DISCONNECT,
				 WIFI_STATE_CONNECTED,
				 WIFI_STATE_GOT_IP};

enum SOFTAP_STATE {SOFTAP_STATE_POWER_ON,
				 SOFTAP_STATE_DISCONNECT,
				 SOFTAP_STATE_CONNECTED,
				 SOFTAP_STATE_GOT_IP};

enum ESP_STATE {ESP_STATE_POWERON,
				ESP_STATE_DETECTING,
                ESP_STATE_RESETING,
				ESP_STATE_CONFIG_WIFI,
				ESP_STATE_CONNECT_AP,
				ESP_STATE_IDLE,
				ESP_STATE_WAIT_TCP,
                ESP_STATE_IDLE_TCP,
				ESP_STATE_BUSY};

enum SOFTAP_SERVER_STATE {SOFTAP_SERVER_STOPPED,
				SOFTAP_SERVER_STARTED};

enum ESP_TX_STATUS_ENUM{ ESP_TX_IDLE,
                         ESP_TX_START,
						 ESP_TX_WAIT,
                         ESP_TX_COMPLETE,
						 ESP_TX_ERROR,
                         ESP_TX_INVALID,
						 ESP_TX_TIMEOUT};


typedef struct
{
    unsigned Valid:1;
    unsigned Unknow:1;
    unsigned AtOk:1;
    unsigned WifiMode:1;
    unsigned Error:1;
    unsigned SetAp:1;
    unsigned SetApIp:1;
    unsigned SetDhcp:1;
    unsigned SetStationIp:1;
    unsigned DisconnectAp:1;
    unsigned ConnectAp:1;
    unsigned WifiConnected:1;
    unsigned WifiDisconnected:1;
    unsigned WifiGotIp:1;
    unsigned CipMode:1;
    unsigned CipMux:1;
    unsigned CipServer:1;
    unsigned TcpConnect:1;
    unsigned TcpClose:1;
    unsigned TcpReceive:1;
    unsigned TxPrompt:1;
    unsigned TxSendOk:1;
}ReceivedCmdStruct;

typedef struct
{
	unsigned char DefaultMode;
	unsigned char Mode;
	enum STATION_STATE Status;
	unsigned char Dhcp;
	unsigned char Configured;
	unsigned char State;
	unsigned Waitflag:1;
	char Ssid[35];
	char Password[65];
	char Ip[16];
	char Gateway[16];
	char SubnetMask[16];
}StationStruct;
//
typedef struct
{
	enum SOFTAP_STATE Status;
	enum SOFTAP_SERVER_STATE Server;
    char Ssid[35];
	char Password[65];
	char Ip[16];
	char Gateway[16];
	char SubnetMask[16];    
}SoftApStruct;
//
typedef struct
{
    unsigned int TimeCounter;
    unsigned char TurnsCounter;
    unsigned int Ton;
    unsigned int Toff;
    unsigned int Delay;
    unsigned char BlinkTurns;
    unsigned char Enable;
    unsigned char State;    
}LedStruct;
//
typedef struct
{
    char RxBuffer[256];
    unsigned int RxSize;
}TcpStruct;
//
typedef struct
{
	enum ESP_STATE State;
    unsigned char SubState;
	//char Command[64];
	char *pTxBuffer;
    char *pRxBuffer;
	unsigned char RxStatus;
	enum ESP_TX_STATUS_ENUM TxStatus;
	unsigned int TxSize;
    unsigned int RxUartTimeout;
	unsigned char Retries;
	StationStruct Station;
	SoftApStruct AccessPoint;
    ReceivedCmdStruct ReceivedCmd;
    LedStruct StatusLed;
    TcpStruct Tcp[4];
    unsigned char CurrentSocketId;
    unsigned int TcpTxSize;
    unsigned char TcpTxState;
    unsigned char TcpTxStart;
    unsigned char TcpTxBuffer[180];
	//
}ESP8266Struct;
//
void ESP8266Init(void);
void ESP8266StateHandler(void);

#endif /* ESP8266_H_ */