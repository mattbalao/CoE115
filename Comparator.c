/* 
 * File:   Comparator.c
 * Author: Kristian
 *
 * Created on April 27, 2019, 12:55 AM
 */
#include "xc.h"

#define FCY 4000000
#include "libpic30.h"

_CONFIG1 (FWDTEN_OFF & JTAGEN_OFF)
_CONFIG2 (POSCMOD_NONE & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_OFF & PLLDIV_NODIV)
_CONFIG3 (SOSCSEL_IO)
 
unsigned int eventCount = 0;

void main(void) {
    
    AD1PCFG = 0xFDFF;
    TRISA = 0xFFFC;
    TRISB = 0x7FFF;
    //TRISAbits.TRISA0 = 0;
    //LATAbits.LATA0 = 1;
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    _RP5R = 30;
    _RP6R = 1;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    
    //TRISBbits.TRISB2 = 1;
    //TRISBbits.TRISB3 = 1; // Disable Digital Output on port pin
    
    IEC1bits.CMIE = 0;
    IFS1bits.CMIF = 0;
    
    CVRCON = 0x84; // CVRef = 1.24 V
    
    //CM1 Setup
    
    CM1CONbits.COE = 1; // Enable output pin
    CM1CONbits.CPOL = 0; // Standard sense. +In High ==> Out High
    CM1CONbits.EVPOL = 2; // Event detected on output edge falling
    CM1CONbits.CREF = 1; // +IN is CVRER
    CM1CONbits.CCH = 1; // -IN is C1INC Pin
    CM1CONbits.CEVT = 0;
    
    //CM3 Setup
    
    CM3CONbits.COE = 1; // Disable output pin
    CM3CONbits.CPOL = 0; // Standard sense. +In High ==> Out High
    CM3CONbits.EVPOL = 2; // Event detected on output edge falling
    CM3CONbits.CREF = 1; // +IN is CVREF
    CM3CONbits.CCH = 0; // -IN is C1INB Pin
    CM3CONbits.CEVT = 0;
    
    //Timer Setup
    
    T1CON = 0x00;
    PR1 = 0x3FFF;
    T1CONbits.TCKPS = 2;
    
    IPC0bits.T1IP = 0x3;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    
    CM1CONbits.CON = 1; // Turn Comparator ON
    CM3CONbits.CON = 1; // Turn Comparator ON
    T1CONbits.TON = 1; // Turn Timer ON
    
    while (1) {
        //LATAbits.LATA1 = !LATAbits.LATA1;
        //__delay_ms(500);
        
        /*if (CMSTATbits.C1EVT){ // Check C1EVT bit
            LATAbits.LATA0 = !LATAbits.LATA0;
        }*/
    }
}

void __attribute__((__interrupt__, __shadow__, no_auto_psv)) _T1Interrupt(void){
    //LATAbits.LATA0 = !LATAbits.LATA0;
    
    if (eventCount == 0){
        CVRCON = 0x8C; // CVRef = 1.24 V
        eventCount = 1;
    }else if(eventCount == 1){
        CVRCON = 0x84; // CVRef = 1.96 V
        eventCount = 0;
    }
    
    IFS0bits.T1IF = 0;
}

