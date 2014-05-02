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
/*Global Variables Declarations*/
unsigned char hours = 12;
unsigned char minutes = 0;
unsigned char seconds = 0;
char time[] = "00:00:00";
 
bool isMorning = false;

/*
** function prototypes
*/ 
void wait_until_key_pressed(void);
void LCD_update_time(void);

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

void LCD_update_time()
{
    unsigned char temp;

    lcd_clrscr();
    lcd_gotoxy(0,1);                    // Go to first character of second line
    itoa( hours/10 , temp, 10);         // Display the hours digits
    lcd_puts(temp);                     // Nifty way to zero pad single digit numbers
    itoa( hours%10 , temp, 10);
    lcd_puts(temp);
    lcd_puts(":");

    itoa( minutes/10 , temp, 10);       // Display the minutes digits
    lcd_puts(temp);
    itoa( minutes%10 , temp, 10);
    lcd_puts(temp);
    lcd_puts(":");

    itoa( seconds/10 , temp, 10);       // Display the seconds digits
    lcd_puts(temp);
    itoa( seconds%10 , temp, 10);
    lcd_puts(temp);
}

int main(void)
{
    
    //Setup the clock
    cli();                              //Disable global interrupts
    TCCR1B |= 1<<CS11 | 1<<CS10;        //Divide by 64
    OCR1A = 15624;                      //Count 15624 cycles for 1 second interrupt
    TCCR1B |= 1<<WGM12;                 //Put Timer/Counter1 in CTC mode
    TIMSK1 |= 1<<OCIE1A;                //enable timer compare interrupt
    sei();                              //Enable global interrupts

    //Setup the I/O for the LED

    DDRD |= (1<<7);                     //Set PortD Pin7 as an output
    PORTD |= (1<<7);                    //Set PortD Pin7 high to turn on LED

    DDRD &=~ (1 << PD2);                /* Pin PD2 input              */
    PORTD |= (1 << PD2);                /* Pin PD2 pull-up enabled    */

    /* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_puts("Hello World\n");
    lcd_puts(time);
    for (;;) {                           /* loop forever waiting for interrupts*/
    }
}

ISR(TIMER1_COMPA_vect)      //Interrupt Service Routine
{
        PORTD ^= (1<<7);                //Use xor to toggle the status LED
        seconds++;
        if(seconds==60)
        {
            seconds = 0;
            minutes++;
        }
        if(minutes==60)
        {
            minutes = 0;
            hours++;
        }
        if(hours==13)
        {
            hours = 1;
        }
        LCD_update_time();
}

