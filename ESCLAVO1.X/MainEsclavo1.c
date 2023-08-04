// File:   MainEsclavo1.c
// Author: Johann Haeussler 21223
// 31.07.23


#pragma config  FOSC    = INTRC_NOCLKOUT
#pragma config  WDTE    = OFF
#pragma config  PWRTE   = OFF
#pragma config  MCLRE   = OFF
#pragma config  CP      = OFF
#pragma config  CPD     = OFF
#pragma config  BOREN   = OFF
#pragma config  IESO    = OFF
#pragma config  FCMEN   = OFF
#pragma config  LVP     = OFF

#pragma config  BOR4V   = BOR40V
#pragma config  WRT     = OFF

#include <stdint.h>
#include <xc.h>
#include "SPIS1.h"
#include "ADC.h"
#define CONT 0xAA
#define _XTAL_FREQ  8000000

uint8_t POT, check,cont;
void setup(void);

void __interrupt() isr(void){
    if (INTCONbits.RBIF==1){               //Interrupciones de botones en PORTB con PULL UP
        if (RB0==0){
            if (RB0==0)       //RB0 incrementa PORTA
                cont++;
        } else if (RB1==0){     //RB1 decrementa PORTA
            if (RB1==0)
                cont--;  
        }
        INTCONbits.RBIF=0;
    }
    else if(PIR1bits.SSPIF == 1){
        check = spiRead();
        if(check==CONT){
            spiWrite(POT);
        }else{
            spiWrite(cont);
        }
        PIR1bits.SSPIF = 0;
    }
}

void main(void) {
    setup();
    while(1){
       POT = ADC_READ(); 
    }
    return;
}

void setup(void){
    //oscilador a 8M Hz
    OSCILLATOR(1);
    
    ANSEL =  0b00100000;
    ANSELH = 0x00;
    
    TRISB = 0b00000011;     //RB1 y RB0 entradas
    PORTB = 0;
    
    //Config. PULL UP
    IOC_INT(0b00000011);     //pines 1 y 2 se realizaran la interrupcion
    
    //Config. ADC
    ADC_INIT(5);            //canal 5
       
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupci�n MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupci�n MSSP
    INTCONbits.RBIE = 1;    //interrupcion pull up 
    INTCONbits.RBIF = 0;
    
    TRISAbits.TRISA5 = 1;       // Slave Select
    
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
 }