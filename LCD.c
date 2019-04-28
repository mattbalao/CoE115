#include "xc.h"

#define FCY 4000000
#include "libpic30.h"

#define ldata LATB                 /* PORTB is used for transmitting data to LCD */

_CONFIG1 (FWDTEN_OFF & JTAGEN_OFF)
_CONFIG2 (POSCMOD_NONE & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_OFF & PLLDIV_NODIV)
_CONFIG3 (SOSCSEL_IO)

int adcvalue = 0;
int ones = 0;
int dec1 = 0;
int dec2 = 0;
int v = 0;
float buf = 0;
int a = 0;
char b = 0x30;
char c = 0;

void lcd_init();
void lcd_clear();
void lcd_command(unsigned char cmd);
void lcd_char(unsigned char dat);
void lcd_string(const char *msg);
void ADC_init(void);
void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void);
void display_value(int adc);
void reset_latbits(void);

void blink_1s(){
    LATAbits.LATA0 = 0;
    __delay_ms(1000);
    LATAbits.LATA0 = 1;
    __delay_ms(1000);
}

void lcd_newchar(char *newchar, char cgram_address){
    // 0b 01 001 000
    int cgram_row = 0;
    cgram_address = (cgram_address & 0x7) << 3;
    lcd_command(cgram_address | 0x40); // Set CGRAM Address to 0b00 0000
    for(cgram_row = 0; cgram_row < 7; cgram_row++){
        lcd_char(newchar[cgram_row]);
    }
}

void main(void) {
    TRISA = 0xFFFE;
    TRISB = 0xFE00;
    LATB = 0xFFFF;
    //char newchar[7] = {0x11, 0x0A, 0x1F, 0x04, 0x1F, 0x04, 0x04};
    char sunflower[7] = {0x0E, 0x1B, 0x11, 0x1B, 0x0E, 0x04, 0x1F};
    char peashooter[7] = {0x08, 0x0C, 0x13, 0x11, 0x0E, 0x04, 0x1F};
    char wallnut[7] = {0x04, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x0E};
    
    
    lcd_init();
    
    lcd_string("XY\nZ");
    
    lcd_newchar(sunflower, 0);
    lcd_newchar(peashooter, 1);
    lcd_newchar(wallnut, 2);
    
    lcd_command(0x80); // Set DDRAM Address to 0b000 0000
    lcd_char(0x00);
    lcd_char(0x01);
    lcd_char(0x02);
    
    //lcd_command(0x80);
    lcd_char('B');
    
    while(1){
       
    }
}

void lcd_init(void){
    LATBbits.LATB4 = 0;
    LATBbits.LATB5 = 0;
    __delay_ms(16);       
    lcd_command(0x3); // Function Set Command
    __delay_ms(5);
    lcd_command(0x3); // Function Set Command
    __delay_us(200);
    lcd_command(0x3); // Function Set Command
    
    lcd_command(0x02); // Return Home
    
    // 0b 001 DL N F x x
    lcd_command(0x2F); // Function Set - 4-bit instruction (DL = 0), Display Lines (N = 1), 5x8 Font (F = 0)
    
    // 0b 0000 1 D C B
    lcd_command(0x0C); // Display Control - Display Switch ON (D = 1), Hide Cursor (C = 0), No Character Blinks (B = 0)
    
    lcd_command(0x01); // Clear Display

    // 0b 0000 01 I/D S
    lcd_command(0x06); // Entry Mode Set -  Increment(Shift Right) (I/D = 1), Shift Display Off (S = 0)
    
    //lcd_command(0x0C);
}

void lcd_clear(void){
    lcd_command(0x01);
}

void lcd_command(unsigned char cmd){           
	LATBbits.LATB4 = 0;
    __delay_us(100);
	LATBbits.LATB5 = 1;                
	__delay_us(100);
    ldata= (ldata & 0xFFF0) | ((0x00F0 & cmd) >> 4 ) ;
   __delay_ms(16);
	LATBbits.LATB5 = 0;
    __delay_us(100);
    
    LATBbits.LATB4 = 0;
    __delay_us(100);
	LATBbits.LATB5 = 1;                
	__delay_us(100);
    ldata= (ldata & 0xFFF0) | (0x000F & cmd); 
   __delay_ms(16);
	LATBbits.LATB5 = 0;
    __delay_us(100);
}

void lcd_char(unsigned char dat){
	LATBbits.LATB4 = 1;
    __delay_us(100);
	LATBbits.LATB5 = 1;                
	__delay_us(100);
    ldata= (ldata & 0xFFF0) | ((0x00F0 & dat) >> 4);
   __delay_ms(16);
	LATBbits.LATB5 = 0;
    __delay_us(100);
    
    LATBbits.LATB4 = 1;
    __delay_us(100);
	LATBbits.LATB5 = 1;                
	__delay_us(100);
    ldata= (ldata & 0xFFF0) | (0x000F & dat);
    __delay_ms(16);
	LATBbits.LATB5 = 0;
    __delay_us(100);
    
    //address = address + 1;
    return;
}

void lcd_string(const char *msg){
	while((*msg)!=0){		
        if (msg[0] == '\n'){
            lcd_command(0xC0);
        }else{
            lcd_char(*msg);
        }
        msg++;	
    }		
}
