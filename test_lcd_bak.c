/*************************************************************************
Title:    testing output to a HD44780 based LCD display.
Author:   Peter Fleury  <pfleury@gmx.ch>  http://jump.to/fleury
File:     $Id: test_lcd.c,v 1.6 2004/12/10 13:53:59 peter Exp $
Software: AVR-GCC 3.3
Hardware: HD44780 compatible LCD text display
          ATS90S8515/ATmega if memory-mapped LCD interface is used
          any AVR with 7 free I/O pins if 4-bit IO port mode is used
**************************************************************************/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include <avr/interrupt.h>
#include <stdbool.h>
/*
** constant definitions
*/
static const PROGMEM unsigned char copyRightChar[] =
{
	0x07, 0x08, 0x13, 0x14, 0x14, 0x13, 0x08, 0x07,
	0x00, 0x10, 0x08, 0x08, 0x08, 0x08, 0x10, 0x00
};
int number = 0;
int hours = 12;
int mins = 0;
int secs = 0;
bool isMorning = false;
/*
** function prototypes
*/ 
void wait_until_key_pressed(void);


void wait_until_key_pressed(void)
{
    unsigned char temp1, temp2;
    unsigned int i;
    
    do {
        temp1 = PIND;                  // read input
        for(i=0;i<65535;i++);
        temp2 = PIND;                  // read input
        temp1 = (temp1 & temp2);       // debounce input
    } while ( temp1 & _BV(PIND2) );
    
    loop_until_bit_is_set(PIND,PIND2);            /* wait until key is released */
}


int main(void)
{
    char buffer[7];
    int  num=134;
    unsigned char i;
    
    //Setup the clock
    cli();                          //Disable global interrupts
    TCCR1B |= 1<<CS11 | 1<<CS10;    //Divide by 64
    OCR1A = 15624;                  //Count 15624 cycles for 1 second interrupt
    TCCR1B |= 1<<WGM12;             //Put Timer/Counter1 in CTC mode
    TIMSK1 |= 1<<OCIE1A;            //enable timer compare interrupt
    sei();                          //Enable global interrupts

    //Setup the I/O for the LED

    DDRD |= (1<<7);                 //Set PortD Pin7 as an output
    PORTD |= (1<<7);                //Set PortD Pin7 high to turn on LED

    DDRD &=~ (1 << PD2);            /* Pin PD2 input              */
    PORTD |= (1 << PD2);            /* Pin PD2 pull-up enabled    */

    /* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);

    for (;;) {                           /* loop forever */
        /* 
         * Test 1:  write text to display
         */

        /* clear display and home cursor */
        lcd_clrscr();
        
        /* put string to display (line 1) with linefeed */
        lcd_puts("Hello World\n");

        /* cursor is now on second line, write second line */
        /*char testString[] = "Test String";*/ 
        /*lcd_puts(number);*/
       
        /* convert interger into string */
        /*itoa( number , buffer, 10); */
        
        /* put converted string to display */
        /* lcd_puts(buffer);*/
        wait_until_key_pressed();
    }
}

ISR(TIMER1_COMPA_vect)      //Interrupt Service Routine
{
        char hourBuffer[7];
        char minsBuffer[7];
        char secsBuffer[7];
        
        PORTD ^= (1<<7);        //Use xor to toggle the LED
        secs++;
        if(secs==60)
        {
            secs = 0;
            mins++;
        }
        if(mins==60)
        {
            mins = 0;
            hours++;
        }
        if(hours==13)
        {
            hours = 1;
        }

        /* put converted string to display */
        lcd_clrscr();
        lcd_gotoxy(3,1);
        itoa( hours , hourBuffer, 10);
        lcd_puts(hourBuffer);

        lcd_gotoxy(5,1);
        lcd_puts(":");
        
        lcd_gotoxy(6,1);
        itoa( mins , minsBuffer, 10);
        lcd_puts(minsBuffer);
        
        lcd_gotoxy(8,1);
        lcd_puts(":");

        lcd_gotoxy(9,1);
        itoa( secs , secsBuffer, 10);
        lcd_puts(secsBuffer);
}

