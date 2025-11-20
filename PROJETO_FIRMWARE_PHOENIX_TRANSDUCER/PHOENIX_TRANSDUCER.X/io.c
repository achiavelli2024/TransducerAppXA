#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"

void IoInit(void)
{
    //Configura os pinos como port digital ou anal�gico
    ANSELBbits.ANSB0 = ANALOG;
    ANSELBbits.ANSB1 = DIGITAL;
    ANSELBbits.ANSB2 = DIGITAL;
    ANSELBbits.ANSB3 = DIGITAL;
    ANSELBbits.ANSB4 = DIGITAL;
    ANSELBbits.ANSB5 = DIGITAL;
    ANSELBbits.ANSB6 = DIGITAL;
    ANSELBbits.ANSB7 = DIGITAL;
    ANSELBbits.ANSB8 = DIGITAL;
    ANSELBbits.ANSB9 = DIGITAL;
    ANSELBbits.ANSB10 = DIGITAL;
    ANSELBbits.ANSB11 = DIGITAL;
    ANSELBbits.ANSB12 = DIGITAL;
    ANSELBbits.ANSB13 = DIGITAL;
    ANSELBbits.ANSB14 = DIGITAL;
    ANSELBbits.ANSB15 = ANALOG;

    ANSELEbits.ANSE6 = DIGITAL;
    ANSELEbits.ANSE7 = DIGITAL;
    ANSELEbits.ANSE0 = DIGITAL;
    ANSELEbits.ANSE1 = DIGITAL;
    ANSELEbits.ANSE3 = DIGITAL;
    ANSELEbits.ANSE2 = DIGITAL;
    ANSELEbits.ANSE4 = DIGITAL;
    ANSELEbits.ANSE5 = DIGITAL;
    
    
    ANSELGbits.ANSG6 = DIGITAL;
    ANSELGbits.ANSG7 = DIGITAL;
    ANSELGbits.ANSG8 = DIGITAL;
    ANSELGbits.ANSG9 = DIGITAL;

    ANSELDbits.ANSD6 = DIGITAL;
    ANSELDbits.ANSD6 = DIGITAL;


//Inicializa os ports de IOs digitais
    
    PWR_ENABLE_TRIS = OUTPUT;
    PWR_ENABLE_PIN = 1;
    LED_GREEN_PIN = ON;       
    LED_GREEN_TRIS = OUTPUT;
    LED_YELLOW_PIN = ON;       
    LED_YELLOW_TRIS = OUTPUT;
    AVCC_ENABLE_TRIS = OUTPUT;
    ENCVCC_ENABLE_TRIS = OUTPUT;
    ENCVCC_ENABLE_PIN = OFF;
    AVCC_ENABLE_PIN = ON;   //Reginaldo 26/08/2020
    POWERGOOD_TRIS = INPUT;
    BAT_STAT1_TRIS = INPUT;
    BAT_STAT2_TRIS = INPUT;
    ON_SW_TRIS = INPUT;
    PWR_CFG_TRIS = INPUT;
    BAT_SENSE_ENABLE_TRIS = OUTPUT;
    BAT_SENSE_ENABLE_PIN = OFF;
    BLUETOOTH_PWR_ENABLE_TRIS = OUTPUT;
    BLUETOOTH_PWR_ENABLE_PIN = OFF;
    #if(SMARTCLICK_ENABLE == 1)
        ADDR0_EXP_TRIS = INPUT;
        ESP8266_RESET_PIN = ON;
        ESP8266_RESET_TRIS = OUTPUT;        
    #else
        ADDR0_EXP_TRIS = INPUT;
        ADDR1_EXP_TRIS = INPUT;
    #endif
    
    ESP8266_MODE_TRIS = OUTPUT;
    ESP8266_MODE_PIN = ON;
    
    TEST_TRIS = OUTPUT;
    TEST_PIN = OFF;
    
//    TRISGbits.TRISG6 = OUTPUT;
//    PORTGbits.RG6 = 1; 
    
    //TRISFbits.TRISF0 = OUTPUT;
    //PORTFbits.RF0 = 0; 
    
//    TRISFbits.TRISF1 = OUTPUT;
//    PORTFbits.RF1 = 0;     
    
//
//Re-mapeamento dos perif�ricos
//
//    //__builtin_write_OSCCONL(OSCCON & ~(1<<6));      //Destrava IO Lock bit
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPINR1bits.INT2R = ENC_A_EXTERNAL_INT;
    RPINR1bits.INT3R = ENC_B_EXTERNAL_INT;
    RPINR19bits.U2RXR = COM2_RX_REMAP_PIN;
    RPINR18bits.U1RXR = COM1_RX_REMAP_PIN;
    COM2_TX_REMAP_PIN;
    COM1_TX_REMAP_PIN;
    //PWM
    ADC_PWM_CLOCK_REMAP_PIN;
    __builtin_write_OSCCONL(OSCCON | (1<<6));       //Trava IO Lock bit
    Nop();
}
//
void AdcIoInit(void)
{
    ADC_CLOCK_TRIS = OUTPUT;
    ADC_CLOCK_PIN = 0;
    //   
    ADC_SYNC_TRIS = OUTPUT;
    ADC_SYNC_PIN = ON;
    //
    ADC_FORMAT_TRIS = OUTPUT;
    ADC_FORMAT_PIN = OFF;
    //
    ADC_MODE_TRIS = OUTPUT;
    ADC_MODE_PIN = OFF;
    //
    ADC_DRDY_TRIS = INPUT;
    //
    //Destrava IO Lock bit
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    ADC_DIN_REMAP_PIN;
    ADC_DCLK_REMAP_PIN;
    RPINR20bits.SDI1R = ADC_DOUT_REMAP_PIN;
    RPINR0bits.INT1R = ADC_EXTERNAL_INT;
    __builtin_write_OSCCONL(OSCCON | (1<<6));
    //Trava IO Lock bit
}
