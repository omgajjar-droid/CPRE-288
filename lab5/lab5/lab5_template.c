/**
 * lab5_template.c
 *
 * Template file for CprE 288 Lab 5
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 *
 * @author Phillip Jones, updated 6/4/2019
 * @author Diane Rover, updated 2/25/2021, 2/17/2022
 */


#include "timer.h"
#include "lcd.h"

#include "uart.h"  // Functions for communicating between CyBot and Putty (via UART1)
                         // PuTTy: Baud=115200, 8 data bits, No Flow Control, No Parity, COM1

#include "cyBot_Scan.h"  // Scan using CyBot servo and sensors


#define REPLACEME 0



int main(void) {
        char buffer[25];
        int count = 0;
        timer_init();
        lcd_init();
        uart_init();

        char data;

        while(1)
        {
            data = uart_receive();

            uart_sendChar(data);

            if(data == '\r' || count == 20)
                {
                    buffer[count] = '\0';
                    uart_sendChar('\n');
                    lcd_clear();
                    lcd_printf("Count = %d\n%s", count, buffer);

                    count = 0;
                }
            else {
                lcd_printf("%c", data);
                buffer[count] = data;
                count++;
                lcd_printf("Count = %d\nChar = %c", count, data);
            }
        }
    }
