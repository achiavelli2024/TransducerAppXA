//#include<xc.h>
#include <p24EP512GP806.h>
//
//Contantes de Ambiente
//
#define PRODUCT_TYPE            1       //0 = Fixo   1 = Rotativo
#define AVG_FILTER_ENABLE       1
#define SMARTCLICK_ENABLE       0
//
#if(PRODUCT_TYPE == 0)
    #define PWR_BUTTON_ENABLE       0
    #define TYPE                    "00"
    #define FIRMWARE_ID             "MSHPHX00"
#elif(PRODUCT_TYPE == 1)
    #define PWR_BUTTON_ENABLE       1
    #define TYPE                    "01"
    #define FIRMWARE_ID             "MSHPHX01"
#endif
//
#define COMM_INTERFACE          0       //0 = USB   1 = BT
#define NVRAM_IC_TYPE           1       //0 = FRAM  1 = EEPROM
//
#define HC06_CONFIG             0       //0 = Operação Normal   1 = Configuração do HC06
//
#define HC06_CONN               1       //0 = Placas antigas via CN1 (Torque Tester)   1 = Placas novas via CN7 (Transdutor Rotativo)
//                                      //2 = ESP8266 // 3 = Coletor de torque
#define ANALOGPWRSOFTSTART      0
//
#define ESP_RESET_ON            0
#define ESP_RESET_OFF           1

#define ESP_POWER_ON            1
#define ESP_POWER_OFF           0

#define TEMPERATURE_SENSOR_ENABLE   0

//
#define RUNMODE                 1               //0= Apenas HiSpeed 1 = Troca Autom�tica
#define SERIAL_NUMBER           "20160900"      //Número de Série da Placa
#define ID                      "ITIB"          //ID da ferramenta
#define MODEL                   "PHOENIX_TRANSDUCER_MSHIMIZU_XXYY"
#define CAPACITY                "000020"
#define FIRMWARE_ID_SIZE        8
#define MODEL_SIZE              32
#define HARDWARE_VERSION        "2.00"          //Versão de Hardware
#define FIRMWARE_VERSION        "1.42"          //Versão de Firmware
#define VERSION_SIZE            4               //Número de caracteres do string de versão
#define SERIAL_SIZE             8               //Número de caracteres do string de número de série
#define TYPE_SIZE               2
#define CAPACITY_SIZE           6
#define ID_SIZE                 4               //Número de caracteres do string de ID
#define SYSTEMTIC               8751            //8751 1ms para 70MHz, prescaler = 8 (Timer 1)
#define SLOWPWMTIC              8751            //1ms para 70MHz, prescaler = 8 (Timer 2)
#define FASTPWMTIC              300//700        //1us para 70MHz, prescaler = 1 (Timer 4)
#define _XTAL_FREQ              12000000        //Frequência do Xtal em Hz
#define STOP                    0
#define RUN                     1
#define OVERFLOW                2
#define INPUT                   1
#define OUTPUT                  0
#define TRUE                    1
#define FALSE                   0
#define ON                      1
#define OFF                     0
#define _ON                     0
#define _OFF                    1
#define RAM_BUF_SIZE            8500            //10000
#define RS485_BAUDRATE          51              //Baud rate do canal serial RS485
#define I2C_BAUDRATE            330             //Baud rate do canal I2C (165 = 400kHz @ 70MHz)330
#define RECEIVE                 0
#define TRANSMIT                1
#define ANALOG                  1
#define DIGITAL                 0
#define PWM_ON                  0b110
#define PWM_OFF                 0
#define PWM_FAST_SOURCE         0b010           //FCY como gerador de clock do PWM
#define PWM_SLOW_SOURCE         0b001           //Timer 2 como gerador de clock do PWM
#define TORQUE_AD_CHANNEL       15
#define SIN_AD_CHANNEL          1
#define COS_AD_CHANNEL          13
#define VREF_AMP_AD_CHANNEL     9
#define VREF_AD_CHANNEL         0
#define TEMP_AD_CHANNEL         3
#define TORQUE_OPEN_AD_CHANNEL  5
#define UART_EMPTY              0
#define UART_BUSY               1
#define UART_FULL               2
#define UART_TX_COMPLETED       3
#define AD_IDLE                 0
#define AD_BUSY                 1
#define AD_SAMPLE_COMPLETED     3
#define AD_FAST_MODE            0
#define AD_STD_MODE             1
#define AD_DELAY                1
#define RX_TIMEOUT              8751
#define ENABLE                  1
#define DISABLE                 0
#define ENABLED                 1
#define BLINK                   2
#define _ENABLED                0
#define _ENABLE                 0
#define _DISABLE                1
#define T_ON                    1
#define T_OFF                   0
#define BUZZER_MIN_FREQ         400
#define BUZZER_MAX_FREQ         4000
#define BUZZER_MAX_TURNS        100
#define BUZZER_MIN_TON          100
#define BUZZER_MAX_TON          65000
#define BUZZER_MIN_TOFF         100
#define BUZZER_MAX_TOFF         65000
#define NVRAM_MIN_ADDRESS       0x0000 //0x400
#define NVRAM_MAX_ADDRESS       0x1E0B
#define NVRAM_MIN_PAGE_SIZE     1
#define NVRAM_MAX_PAGE_SIZE     64
#define LED_MIN_TON             50
#define LED_MAX_TON             0xFFFF
#define LED_MIN_TOFF            50
#define LED_MAX_TOFF            0xFFFF
#define LED_MAX_PERIOD          0xFFFF
#define HORARIO                 0
#define ANTIHORARIO             1
#define NAO_ACIONADO            0
#define ACIONADO                1
#define GATILHO_DEBOUNCE        5
#define REVERSAO_DEBOUNCE_PRESS 25
#define REVERSAO_DEBOUNCE_RELEASE 25
#define TEMP_THRESHOLD_MAX      1312
#define TEMP_THRESHOLD_MIN      1161
#define MAX_SIN_COS_OFFSET      1843
#define MIN_SIN_COS_OFFSET      1570
#define DERIVATE_DISABLED       2
#define DERIVATE_READY          1
#define DERIVATE_NOT_READY      0
#define UP                      2
#define DOWN                    1
#define STABLE                  0
//
#define RX_BUFFER_SIZE          1200//512//150
#define TX_BUFFER_SIZE          1200//630//1100//150
#define TX_BUFFER_SIZE1         64//150
#define UART_IDLE               0
#define UART_PREFIX             1
#define UART_DATA               3
#define UART_CMD_RECEIVED       4
#define CMD_PREFIX_1            0xFA
#define CMD_PREFIX_2            0xFF
#define FAST_RX_MAX_BUF_SIZE    7
#define SLOW_RX_MAX_BUF_SIZE    255//64
#define FASTCOMM_RX_TIMEOUT     2100            //1750
#define UART_FAST_MODE          0
#define UART_SLOW_MODE          1
#define FAST_RESPONSE_PREFIX    0xA0
#define SLOW_COMM_BAUD          113             //455 = 9600bps    227 = 19200bps 38 =  115200

#define BAUD_1200               3640
#define BAUD_2400               1820
#define BAUD_4800               910
#define BAUD_9600               455
#define BAUD_19200              227
#define BAUD_38400              113
#define BAUD_57600              76
#define BAUD_115200             38
#define BAUD_230400             19

#define BT_CONFIG_COMM_BAUD     113
#define BT_COMM_BAUD            455
#define FAST_COMM_BAUD          0x0000          //0 = 4375000 bps
#define STX                     '['
#define ETX                     ']'
#define GRAPH_STX               '<'
#define GRAPH_ETX               '>'
#define SLOWCOMM_IDLE           0
#define SLOWCOMM_STX_RECEIVED   1
#define SLOWCOMM_BUSY           2
#define SLOWCOMM_JOB            3
#define SLOWCOMM_TX_EMPTY       0
#define SLOWCOMM_TX_BUSY        1
#define SLOWCOMM_TX_DONE        2
#define SLOWCOMM_RX_TIMEOUT     50             //Timeout de espera por resposta em modo SlowComm (ms)
#define NUM_SAMPLES             3
#define TRANSDUCER_TEST_SAMPLES 2000
#define FASTCOMM_TIMEOUT        500            //Timeout de espera por comando em modo FastComm (ms)
#define LED_TEST_DELAY          1500
#define ENCODER_CW              0
#define ENCODER_CCW             1
#define DATALOG_SAMPLES         5
#define TRACK_SAMPLES           5//1
#define AVERAGE_SAMPLES         1//25
#define DERIVATIVE_SAMPLES      2
//
#define BUTTON_FREE             0b00            //Bot�o n�o pressionado
#define BUTTON_RELEASE          0b01            //Momento em que o bot�o foi pressionado
#define BUTTON_PRESS            0b10            //Momento em que o bot�o foi solto BUTTON_RELEASE
#define BUTTON_HOLD             0b11            //Bot�o mantido pressionado
//
//Timers de software
//
#define DELAYTIMER              0
#define FINAL_THRESHOLD_TIMER   1
#define LED_TIMER               2
#define TIMEOUT_TIMER           3
#define ACQ_TIMER               4
#define PWR_BUTTON_TIMER        5
#define AUTO_PWR_OFF_TIMER      6
#define BAT_SENSE_TIMER         7
#define CRITICAL_BAT_TIMER      8
#define CLICK_TORQUE_TIMER      9
#define UART_TIMER              10
#define ESP_TIMER               11
#define ESP_LEDTIMER            12
#define TIMERS_QUANTITY         13               //Quantidades de estruturas de timers
//
//Estados
//
#define INITIAL                 0
#define IDLE                    1
#define READY                   2
#define ERROR                   3
#define FASTCOMM                4
#define SLOWCOMM                5
//
//
#define FASTCOMM_INITIAL        0
#define FASTCOMM_SETUP          1
#define FASTCOMM_READY          2
#define FASTCOMM_ERROR          3
//
//
#define SLOWCOMM_INITIAL        0
#define SLOWCOMM_READY          1
//
//Frequ�ncia do LED da placa para debug
//
#define LED_BOARD_INITIAL       500
#define LED_BOARD_READY         500
#define LED_BOARD_ERROR         1000
#define LED_BOARD_FASTCOMM      100
#define LED_BOARD_SLOWCOMM      500
//
#define LED_TORQUE_LOW          1
#define LED_TORQUE_OK           2
#define LED_TORQUE_HI           3
#define LED_CW                  4
#define LED_CCW                 5
#define LED_MANUT               6
#define LED_ALL                 0x0A
//
//
#define LED_OFF                 0
#define LED_ON                  1
#define LED_BLINK               2
//
//Lista de Erros
//
//#define NO_ERROR                0x0000

//Erros de comando
#define NO_ERROR                0x00
#define CRC_ERROR               0x01
#define SINTAX_ERROR            0x02
#define INVALID_CMD             0x03
#define DEVICE_NOT_READY        0x04

#define RESPONSE_ID             "ID"          //ID
#define RESPONSE_ID_SIZE        6

#define RESPONSE_SN             "SN"
#define RESPONSE_SN_SIZE        10

#define RESET_CMD                           0x5253      //RS
#define REQUEST_ID                          0x4944      //ID
#define REQUEST_TORQUE                      0x5451      //TQ
#define WRITE_ACQ_CONFIG                    0x5341      //SA
#define READ_ACQ_CONFIG                     0x4741      //GA
#define READ_DEVICE_INFO                    0x4449      //DI
#define READ_DEVICE_STATUS                  0x4453      //DS
#define READ_ACQ_STATUS                     0x4C53      //LS
#define NEW_ACQ                             0x4E41      //NA
#define GET_DATA                            0x4744      //GD
#define ZERA_OFFSET                         0x5A4F      //ZO
#define CALIB_WRITE                         0x4357      //CW
#define REQUEST_NVRAM_PAGE_WRITE            0x5057      //PW
#define REQUEST_NVRAM_PAGE_READ             0x5052      //PR
#define WRITE_DEVICE_SETUP                  0x5753      //WS
#define READ_COUNTERS                       0x5243      //RC
//
#define WRITE_CAPACITY                      0x5843      //XC
#define WRITE_MODEL                         0x584D      //XM
#define WRITE_SERIALNUMBER                  0x5853      //XS
#define WRITE_BT_CONFIG                     0x5842      //XB
#define CLEAR_COUNTER                       0x5854      //XT
#define WRITE_CLICK_SETUP                   0x4353      //CS
#define READ_CLICK_SETUP                    0x4352      //CR
#define WRITE_ACQ_CONFIG_2                  0x5342      //SB
#define WRITE_ACQ_CONFIG_3                  0x5343      //SC
#define READ_ACQ_CONFIG_2                   0x4742      //GB
#define READ_ACQ_CONFIG_3                   0x4743      //GC
#define WRITE_TORQUE_OFFSET                 0x534F      //SO
#define GET_DATA_ASCII                      0x4758      //GX
//
//Mapeamento de pinos
//
#define LED_GREEN_PIN           PORTBbits.RB1       
#define LED_GREEN_TRIS          TRISBbits.TRISB1

#define LED_YELLOW_PIN          PORTBbits.RB2       
#define LED_YELLOW_TRIS         TRISBbits.TRISB2

#define AVCC_ENABLE_PIN         PORTEbits.RE1       
#define AVCC_ENABLE_TRIS        TRISEbits.TRISE1

#define ENCVCC_ENABLE_PIN       PORTEbits.RE2       
#define ENCVCC_ENABLE_TRIS      TRISEbits.TRISE2

#define PWR_ENABLE_PIN          PORTEbits.RE5       
#define PWR_ENABLE_TRIS         TRISEbits.TRISE5

#define ON_SW_PIN               PORTFbits.RF6       //1 = ON_SW Acionado
#define ON_SW_TRIS              TRISFbits.TRISF6

#define PWR_CFG_PIN             PORTBbits.RB4       
#define PWR_CFG_TRIS            TRISBbits.TRISB4

#define ENCODER_A_PIN           PORTDbits.RD8       //0 = Gatilho acionado
#define ENCODER_A_TRIS          TRISDbits.TRISD8

#define ENCODER_B_PIN           PORTDbits.RD9       
#define ENCODER_B_TRIS          TRISDbits.TRISD9

#define ADC_CLOCK_PIN           PORTDbits.RD3       
#define ADC_CLOCK_TRIS          TRISDbits.TRISD3

#define ADC_SYNC_PIN            PORTDbits.RD1       
#define ADC_SYNC_TRIS           TRISDbits.TRISD1

#define ADC_FORMAT_PIN          PORTCbits.RC14       
#define ADC_FORMAT_TRIS         TRISCbits.TRISC14

#define ADC_MODE_PIN            PORTCbits.RC13       
#define ADC_MODE_TRIS           TRISCbits.TRISC13

#define ADC_DRDY_PIN            PORTDbits.RD11       
#define ADC_DRDY_TRIS           TRISDbits.TRISD11

#define ONE_WIRE_BUS_PIN        PORTBbits.RB3
#define ONE_WIRE_BUS_TRIS       TRISBbits.TRISB3

#define POWERGOOD_PIN           PORTDbits.RD4
#define POWERGOOD_TRIS          TRISDbits.TRISD4

#define BAT_STAT1_PIN           PORTBbits.RB9
#define BAT_STAT1_TRIS          TRISBbits.TRISB9

#define BAT_STAT2_PIN           PORTBbits.RB8
#define BAT_STAT2_TRIS          TRISBbits.TRISB8

#define BAT_SENSE_ENABLE_PIN    PORTBbits.RB5
#define BAT_SENSE_ENABLE_TRIS   TRISBbits.TRISB5

#if(HC06_CONN == 3)

#define BLUETOOTH_PWR_ENABLE_PIN    PORTEbits.RE3
#define BLUETOOTH_PWR_ENABLE_TRIS   TRISEbits.TRISE3

#else

#define BLUETOOTH_PWR_ENABLE_PIN    PORTFbits.RF1
#define BLUETOOTH_PWR_ENABLE_TRIS   TRISFbits.TRISF1

#endif

#define RTS_EXP_PIN             PORTFbits.RF1
#define RTS_EXP_TRIS            TRISFbits.TRISF1

#define ADDR0_EXP_PIN           PORTGbits.RG7
#define ADDR0_EXP_TRIS          TRISGbits.TRISG7

#define ADDR1_EXP_PIN           PORTBbits.RB11
#define ADDR1_EXP_TRIS          TRISBbits.TRISB11

#define ESP8266_MODE_PIN        PORTBbits.RB10
#define ESP8266_MODE_TRIS       TRISBbits.TRISB10

#define ESP8266_RESET_PIN        PORTBbits.RB11
#define ESP8266_RESET_TRIS       TRISBbits.TRISB11

#define TEST_PIN                PORTDbits.RD6
#define TEST_TRIS               TRISDbits.TRISD6
//
//Pinos mape�veis
//
#define PWR_ENABLE_REMAP_PIN    RPOR6bits.RP85R = 0b010001

#define ADC_CLOCK_REMAP_PIN     RPOR1bits.RP67R = 0b110001

#define ADC_DOUT_REMAP_PIN      0b1001010           //RPI74

#define ADC_DIN_REMAP_PIN       RPOR1bits.RP66R = 0b000101

#define ADC_DCLK_REMAP_PIN      RPOR0bits.RP64R = 0b000110

#define ADC_EXTERNAL_INT        0b1001011   //RPI75

#define ENC_A_EXTERNAL_INT      0b1001000   //RPI72

#define ENC_B_EXTERNAL_INT      0b1001001   //RPI73

//#define TORQUE_HI_REMAP_PIN     RPOR3bits.RP70R = 0b010110; 

#define ADC_PWM_CLOCK_REMAP_PIN RPOR1bits.RP67R = 0b010110;

#if COMM_INTERFACE == 1
//COM1
#define COM2_RX_REMAP_PIN       0b1010110   //RPI86
#define COM2_TX_REMAP_PIN       RPOR14bits.RP120R = 0b000011;//0b000001        //RP120
//COM2
#define COM1_RX_REMAP_PIN       0b1010100   //RP84
#define COM1_TX_REMAP_PIN       RPOR13bits.RP118R = 0b000011

#elif COMM_INTERFACE == 0

    #if(HC06_CONN == 0)
    //COM1   BLUETTOOTH             1010110
    #define COM1_RX_REMAP_PIN       0b1010110   //RPI86
    #define COM1_TX_REMAP_PIN       RPOR14bits.RP120R = 0b000001;//0b000001        //RP120
    //COM2
    #define COM2_RX_REMAP_PIN       0b1010100   //RP84
    #define COM2_TX_REMAP_PIN       RPOR13bits.RP118R = 0b000011
    #elif(HC06_CONN == 1)
    //COM1   BLUETTOOTH             1010110
    #define COM1_RX_REMAP_PIN       0b1100101   //RP101  1100101
    #define COM1_TX_REMAP_PIN       RPOR7bits.RP96R = 0b000001;//0b000001        //RP96
    //COM2
    #define COM2_RX_REMAP_PIN       0b1010100   //RP84
    #define COM2_TX_REMAP_PIN       RPOR13bits.RP118R = 0b000011
    #elif (HC06_CONN == 2)
    //COM1   ESP8266                 1010110
    #define COM1_RX_REMAP_PIN       0b0101101   //RPI45  0101101
    #if(SMARTCLICK_ENABLE == 0)
        #define COM1_TX_REMAP_PIN       RPOR9bits.RP100R = 0b000001;//0b000001        //RP100
    #else
        #define COM1_TX_REMAP_PIN       RPOR7bits.RP96R = 0b000001;
    #endif
    //COM2
    #define COM2_RX_REMAP_PIN       0b1010100   //RP84
    #define COM2_TX_REMAP_PIN       RPOR13bits.RP118R = 0b000011
    #elif (HC06_CONN == 3)
    #define COM1_RX_REMAP_PIN       0b1100001   //RP97  
    #define COM1_TX_REMAP_PIN       RPOR7bits.RP96R = 0b000001;//0b000001        //RP96
    //COM2
    #define COM2_RX_REMAP_PIN       0b1010100   //RP84
    #define COM2_TX_REMAP_PIN       RPOR13bits.RP118R = 0b000011
    #endif
#endif

//#define CAN_RX_PIN          96
//#define CAN_RX_TRIS         TRISFbits.TRISF0
//#define CAN_TX_PIN          RPOR7bits.RP97R = 0b001110
//#define CAN_TX_TRIS         TRISFbits.TRISF1
//
//PWM (Output Compare)
//
#define ADC_CLOCK_OC        7               //Canal Output Compare correspondente
//#define LED_TORQUE_LOW_OC   2               //Canal Output Compare correspondente
//#define LED_TORQUE_HI_OC    7               //Canal Output Compare correspondente
//#define LED_TORQUE_OK_OC    6               //Canal Output Compare correspondente
//#define LED_CW_OC           5               //Canal Output Compare correspondente
//#define LED_CCW_OC          3               //Canal Output Compare correspondente
//#define LED_MANUT_OC        4               //Canal Output Compare correspondente
//#define BUZZER_OC           8               //Canal Output Compare correspondente
//#define LED_BOARD           1               //Canal Output Compare correspondente
#define PWR_ENABLE_OC       2
#define GREEN_LED_OC        2
//
//

//
typedef struct TimerStruct 
{
    unsigned long Counter;
    unsigned char State;
}Timer32bits;

typedef struct
{
    unsigned char Status;
    unsigned int Size;
    unsigned int Pointer;
    unsigned char Data[TX_BUFFER_SIZE];
}SerialTxBufferStruct;
//
typedef struct
{
    unsigned char Status;
    unsigned int Size;
    unsigned int Pointer;
    unsigned char Data[TX_BUFFER_SIZE1];
}SerialTxBufferStruct1;

//
typedef struct
{
    unsigned int StartIndex;
    unsigned char Size;
    unsigned char Step;
    
}SendLogStruct;
//
typedef struct
{
    long Sample[AVERAGE_SAMPLES];
    unsigned int Pointer;
    long Result;
}MovAverageStruct;
//
typedef struct
{
    unsigned char Mode;
    unsigned char Status;
    unsigned char Sample;
    unsigned char Channel;
    unsigned int Value[10];
}AnalogChannelStruct;
//
//
typedef struct
{
    unsigned char Status;
    unsigned int Size;//
    unsigned int MessageSize;
    unsigned int Pointer;//
    unsigned char Error;
    unsigned char Timeout;
    unsigned int TimeoutCounter;
    unsigned long Time;
    unsigned long Requests;
    unsigned long Responses;
    unsigned long TimeoutRequest;
    unsigned long CrcMismatch;
    unsigned long SintaxMismatch;
    unsigned int Blink;
    unsigned int Retries;
    unsigned char Data[RX_BUFFER_SIZE];
}SerialRxBufferStruct;
//
//
typedef struct
{
    unsigned char Enable;
    unsigned char State;
    unsigned char Turns;
    unsigned char TurnsCounter;
    unsigned int Freq;
    unsigned int Ton;
    unsigned int Toff;
    unsigned int TimeCounter;
}BuzzerStruct;

typedef struct
{
    unsigned char Status;
    unsigned int Ton;
    unsigned int Toff;
    unsigned int Period;
    
}PwmLedStruct;
//
typedef struct
{
    unsigned Status:1;
    unsigned Anterior:1;
    unsigned Sentido:1;
    //unsigned Pin:1;
    unsigned char Counter;
}GatilhoStruct;
//
typedef struct
{
    unsigned Status:1;
    unsigned Anterior:1;
    unsigned StatusAnt:1;
    unsigned AutoReturn:1;
    unsigned char Modo;
    unsigned char State;
    unsigned long Counter;
    unsigned int OnCounter;
    unsigned int OffCounter;
}ReversaoStruct;
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
}BlinkLedStruct;

typedef struct
{
    int Angle;
    unsigned char Torque[3];
 }DataStruct;

//typedef struct
//{
//    unsigned int Size;
//    unsigned int Index;
//    unsigned int Data[RAM_BUF_SIZE];
//}AngleStruct;

typedef struct
{
    unsigned char Name[16];
    unsigned char Id[ID_SIZE+1];
    unsigned char SerialNumber[SERIAL_SIZE+1];
    unsigned char Model[MODEL_SIZE+1];
    unsigned char HwVersion[VERSION_SIZE+1];
    unsigned char FwVersion[VERSION_SIZE+1];
    unsigned char Type[TYPE_SIZE+1];
    unsigned char Capacity[CAPACITY_SIZE+1];
    //
    unsigned char State;
    unsigned char ErrorCode;
    unsigned int Temperature;
    unsigned char CommInterface;        //0 = RS232/USB; 1 = RS485; 2 = BlueTooth
    unsigned char PowerSource;          //0 = External DC Supply; 1 = Li Battery;
    unsigned char PowerState;           //
    unsigned int PowerVoltage;          //
    unsigned char BatState;
    unsigned long AutoPowerOff;          //Tempo em segundos para desligamento
    unsigned char AnalogPowerState;
    unsigned char EncoderPowerState;
    unsigned int LastReset;
    unsigned long Alive;
    //
    unsigned char CommAct;
    unsigned char CommActState;
    //
    unsigned char AcquisitionStatus;    //0 = DESLIGADO; 1 = LIGADO
    unsigned char AcquisitionType;      //0 = DISABLED; 1 = TORQUE; 2 = TORQUE + ANGLE
    unsigned char AcquisitionReady;     //0 = DISABLED; 1 = ENABLED 
    unsigned char AcquisitionTimeStep;
    unsigned int  AcquisitionIndex;
    unsigned int AcquisitionPeakIndex;
    unsigned int AcquisitionTHFIndex;
    unsigned int AcquisitionSize;
    
    unsigned int AcquisitionFirstPeakIndex;
    long AcquisitionFirstPeak;
        
    long AcquisitionMaxTorque;
    long AcquisitionPeakAngle;
    unsigned char AcquisitionDirection;
    unsigned char AcquisitionExpectedDirection;
    long InitialThreshold;
    long FinalThreshold;
    unsigned long AcquisitionTimeout;
    unsigned long AcquisitionFinalTimeout;
    unsigned int FilterFreq;
    unsigned long FilterPeriod;
    unsigned int FilterPwm;
    unsigned long TorqueSensitivity;
    unsigned long AngleSensivity;
    long Offset;
    long CalibTimeStamp;
    long CalibData;
    long TransducerMaxPositive;
    long TransducerMaxNegative;
//    
    unsigned long AdFactor;             
    unsigned int EncoderRes;            //Resolu��o do encoder em pontos por revolu��o
    unsigned long CycleCounter;
    unsigned long OvertorqueCounter;
    unsigned long MaxAppliedTorque;
    unsigned char MaxAppliedTorqueChecked;
    long OverTorqueLimit;
    long NominalTorque;
    unsigned char ToolType;
    unsigned char BlueToothTrasparent;
    unsigned char FastScan;
    unsigned int ClickFall;
    unsigned int ClickRise;
    unsigned int ClickWidth;
    unsigned int AcquisitionDelayTimeout;
    unsigned int AcquisitionResetTimeout;
    unsigned char AcquisitionDelayFlag;
    
    unsigned char AcqTrigger;
    unsigned char ExpAddress;
}DeviceStruct;
//
typedef struct
{
    long PulseAcc;
    unsigned char PinState;
    unsigned char PrevPinState;
    unsigned Direction:1;
    
}EncoderStruct;
//
typedef struct
{
    int Temperature;
    unsigned char UserByte1;
    unsigned char UserByte2;
    unsigned char   CountRemain;
    unsigned char   CountPerC;
    unsigned char   Crc;
    unsigned char   Status;
    unsigned char   FamilyCode;
    unsigned char   SerialNumber[6];
}DS18S20Struct;
//
typedef struct
{
    unsigned Mode:1;
//    unsigned Anterior:1;
//    unsigned Sentido:1;
    unsigned char State;
}PwrButtonStruct;
//
typedef struct
{
    unsigned long Acummulated;
    unsigned char index;
    unsigned char State;
    //unsigned int Value; 
}BatVoltageStruct;
//
typedef struct
{
   long Samples[DERIVATIVE_SAMPLES];
   unsigned int Count;
   long Result;
   unsigned char Status;
   long MinResult;
   long MaxResult;
}DerivativeStruct;
//

   
#define CLICK_SEARCH                0
#define CLICK_RISE_WAIT             1
#define CLICK_DETECTED              2

#define CLICK_SAMPLES               5

#define CLICK_SAMPLE_LIMIT          25
#define CLICK_PERCENTAGE_RISE       25
#define CLICK_PERCENTAGE_FALL       92
#define CLICK_PULSE_MAX_WIDTH       20  //Em amostragens       

typedef struct
{
    unsigned char Direction;
    unsigned char State;
    unsigned int Count;
    long PeakValue;
    long PrevValue;
    long CurrentValue;
    long Trigger;
    unsigned int SampleIndex;
    long Samples[CLICK_SAMPLES];
    
    
}ClickStruct;



//
#define USB_INTERFACE               0
#define BLUETOOTH_INTERFACE         1
#define WIFI_INTERFACE              2

#define COMM_LED_BLINK              5
#define COMM_LED_RATE               50
#define STATUS_LED_NORMAL           0
#define STATUS_LED_RXTX             1

#define TORQUE_ACQ                  0
#define TORQUEANGLE_ACQ             1

#define MODE_DISABLE                0
#define MODE_TRACK                  1
#define MODE_DATALOG                2

#define EXTERNAL_DC                 1
#define LI_BATTERY                  2

#define ACQ_NOT_READY               0
#define ACQ_READY                   1

#define ACQ_IDLE                    0
#define ACQ_THR                     1
#define ACQ_FINALTIMEOUT            2
#define ACQ_FINISHED                3
#define ACQ_DELAY_TIME              4
#define ACQ_RESET_TIME              5

#define CW                          0
#define CCW                         1
#define CWCCW                       2

#define THI_MIN                     0
#define THI_MAX                     0x7FFFFF
#define THF_MIN                     0
#define THF_MAX                     0x7FFFFF
#define THT_MIN                     10
#define THT_MAX                     10000
#define TIMESTEP_MIN                1
#define TIMESTEP_MAX                100
#define MAX_GET_DATA_SIZE           100
#define FILTER_FREQ_MIN             100
#define FILTER_FREQ_MAX             5000
#define FILTER_FREQ_DEFAULT         10500
#define FILTER_PWM_STEP             142
#define MIN_AUTO_PWROFF_TIMER       30
#define MAX_AUTO_PWROFF_TIMER       3600
#define DEFAULT_AUTO_PWROFF_TIMER   300
#define TORQUE_FULL_NOMINAL_SCALE   3355443
#define TORQUE_OPEN_TRANSDUCER      2 * TORQUE_FULL_NOMINAL_SCALE    
#define TORQUE_MAX_OFFSET_ALLOWED   (TORQUE_FULL_NOMINAL_SCALE)//era 10
#define TORQUE_ZERO_OFFSET          (TORQUE_FULL_NOMINAL_SCALE / 100)*2
#define DERIVATE_THRESHOLD          -10000//-25000
#define OFFSET_SAMPLES              20
#define ACQ_DELAY_DEFAULT           0
#define ACQ_RESET_TIMEOUT_DEFAULT   0
#define ACQ_DELAY_MIN               0
#define ACQ_DELAY_MAX               5000
#define ACQ_RESET_TIMEOUT_MIN       0
#define ACQ_RESET_TIMEOUT_MAX       5000


#define ONOFF_ALWAYS_ON             0
#define ONOFF_PWR_BUTTON            1

#define PWR_BUTTON_ON_PRESS_TIME    1000
#define PWR_BUTTON_OFF_PRESS_TIME   2500

#define PWR_BUTTON_IDLE             0
#define PWR_BUTTON_PRESSED          1
#define PWR_BUTTON_RELEASE          2

#define PWR_SOURCE_BAT              1
#define PWR_SOURCE_EXT              0

#define BAT_FAULT                   0b00
#define BAT_CHARGING                0b01
#define BAT_CHARGE_COMPLETE         0b10
#define BAT_NOT_PRESENT             0b11

#define BAT_NORMAL                  4
#define BAT_LOW                     5
#define BAT_CRITICAL                6

#define BAT_LOW_LEVEL               3250
#define BAT_CRITICAL_LEVEL          3100

#define BAT_AD_CHANNEL              15
#define BAT_AD_MULTIPLIER           1148
#define BAT_AD_DIVIDER              1000
#define BAT_AD_SAMPLES              100

#define BAT_SENSE_IDLE              0
#define BAT_SENSE_ENABLE            1
#define BAT_SENSE_SAMPLE            2

#define BAT_SENSE_IDLE_TIMEOUT      10000
#define BAT_SENSE_ENABLE_TIMEOUT    25
#define BAT_CRITICAL_TURNOFF_TIMEOUT 30000
#define COM1                        1
#define COM2                        2
#define ETH                         3

//#define NO_ERROR                  0
#define TRANSDUCER_ERROR            1
#define OFFSET_ERROR                2

#define BT_NORMAL_MODE              0
#define BT_TRANSPARENT_MODE         1
#define BT_SEARCH_MODE              2

//Click Setup
//
#define DEFAULT_CLICK_FALL          7
#define DEFAULT_CLICK_RISE          95
#define DEFAULT_CLICK_WIDTH         20
#define MAX_CLICK_FALL              99
#define MIN_CLICK_FALL              1
#define MAX_CLICK_RISE              150
#define MIN_CLICK_RISE              1
#define MAX_CLICK_WIDTH             50
#define MIN_CLICK_WIDTH             3

#define LOG_DATA_TOTAL_SIZE     8200

#define LOG_DATA_BLOCK0_SIZE    (LOG_DATA_TOTAL_SIZE - LOG_DATA_BLOCK1_SIZE)
#define LOG_DATA_BLOCK0_ADDR    0x1900
//#define LOG_DATA_BLOCK0_ADDR    (LOG_DATA_BLOCK1_ADDR - (LOG_DATA_BLOCK0_SIZE * 5))

#define LOG_DATA_BLOCK1_SIZE    4096
#define LOG_DATA_BLOCK1_ADDR    0x9000


/*
    1: apertadeira a cabo
    2: apertadeira de impulso
    3: torq estalo
    4: torq digital/analogico
    5: apertadeira pneumática                                         
    6: apertadeira a bateria
    7: apertadeira a bateria transdutorizada
 */

#define TIPO_NAO_DEFINIDO                0x00
#define TIPO_APERTADEIRA_CABO            0x01
#define TIPO_APERTADEIRA_IMPULSO         0x02
#define TIPO_TORQUIMETRO_ESTALO          0x03
#define TIPO_TORQUIMETRO_DIGITAL         0x04
#define TIPO_APERTADEIRA_PENUMATICA      0x05
#define TIPO_APERTADEIRA_BATERIA         0x06
#define TIPO_APERTADEIRA_BATERIA_TRANSD  0x07
#define TIPO_APERTADEIRA_SHUT_OFF        0x08
//
//NVRAM
//

#define NVRAM_BASE_ADDRESS          NVRAM_MAX_ADDRESS + 1
#define NVRAM_PAGE_SIZE             128
//
#define NVRAM_PAGE0_START           NVRAM_MAX_ADDRESS + 1 + 8
#define NVRAM_PAGE1_START           NVRAM_PAGE0_START + NVRAM_PAGE_SIZE
#define NVRAM_PAGE2_START           NVRAM_PAGE1_START + NVRAM_PAGE_SIZE
#define NVRAM_PAGE3_START           NVRAM_PAGE2_START + NVRAM_PAGE_SIZE

//NVRAM PAGE 0
#define NVRAM_SERIAL_NUMBER         NVRAM_MAX_ADDRESS + 1           //8 bytes  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07

//NVRAM PAGE 1
#define NVRAM_TORQUE_SENSITIVITY    NVRAM_PAGE0_START + 0x00   //8 bytes  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
#define NVRAM_ANGLE_SENSITIVITY     NVRAM_PAGE0_START + 0x08   //8 bytes  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
#define NVRAM_CALIB_TIMESTAMP       NVRAM_PAGE0_START + 0x10   //8 bytes  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
#define NVRAM_CALIB_DATA            NVRAM_PAGE0_START + 0x18   //8 bytes  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
#define NVRAM_TYPE                  NVRAM_PAGE0_START + 0x20   //2 bytes  0x20, 0x21
#define NVRAM_CAPACITY              NVRAM_PAGE0_START + 0x22   //6 bytes  0x22, 0x23, 0x24, 0x25, 0x26, 0x27
#define NVRAM_MODEL                 NVRAM_PAGE0_START + 0x28   //32 bytes 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F
                                                               //8 bytes  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
                                                               //8 bytes  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
                                                               //8 bytes  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47
#define NVRAM_CYCLE_COUNTER         NVRAM_PAGE0_START + 0x48   //8 bytes  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F
#define NVRAM_OVERTORQUE_COUNTER    NVRAM_PAGE0_START + 0x50   //8 bytes  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57
#define NVRAM_MAX_APPLIED_TORQUE    NVRAM_PAGE0_START + 0x58   //8 bytes  0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F
#define NVRAM_AUTO_PWR_OFF_TIMER    NVRAM_PAGE0_START + 0x60   //8 bytes  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67
#define NVRAM_TORQUE_OFFSET         NVRAM_PAGE0_START + 0x68   //8 bytes  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F
#define NVRAM_CLICK_FALL            NVRAM_PAGE0_START + 0x70   //2 bytes  0x70, 0x71
#define NVRAM_CLICK_RISE            NVRAM_PAGE0_START + 0x72   //2 bytes  0x72, 0x73
#define NVRAM_CLICK_WITDH           NVRAM_PAGE0_START + 0x74   //2 bytes  0x74, 0x75



