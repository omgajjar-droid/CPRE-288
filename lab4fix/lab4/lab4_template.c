/**
 * lab4_template.c
 *
 * Template file for CprE 288 lab 4
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 */

#include "button.h"
#include "Timer.h"
#include "lcd.h"
#include "cyBot_uart.h"  // Functions for communicating between CyBot and Putty (via UART)
                         // PuTTy: Baud=115200, 8 data bits, No Flow Control, No Parity, COM1



void cyBot_sendString(char *str)
{
    while(*str != '\0')
    {
        cyBot_sendByte(*str);
        str++;
    }
}


int main(void)
{
    timer_init();
    lcd_init();
    button_init();
    cyBot_uart_init();

    int button;

    while(1)
    {
        button = button_getButton();

        if(button != 0)
        {
            lcd_clear();
            lcd_printf("Button %d", button);

            if(button == 1)
                cyBot_sendString("Button 1 thinks Ian's stinky\r\n");
            else if(button == 2)
                cyBot_sendString("Button 2 thinks Michael is stinky\r\n");
            else if(button == 3)
                cyBot_sendString("Button 3 has no opinons\r\n");
            else if(button == 4)
                cyBot_sendString("Button 4 was NOT in Tiananmen Square on June 4, 1989\r\n");

            timer_waitMillis(300);
        }
    }
}
