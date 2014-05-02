int main(void)
{
    lcd_init(LCD_DISP_ON);   /* initialize display, cursor off */
        while (1)               /* loop forever */
        {                           
            lcd_clrscr();                   /* clear the screen*/
            lcd_puts(“test program”);  /* displays the string on lcd*/
            _delay_ms(50);
        }   
} 
