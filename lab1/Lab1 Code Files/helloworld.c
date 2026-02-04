/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 *
 * updated: phjones 9/3/2019
 * Description: Added timer_init call, and including Timer.h
 */

#include "Timer.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BANNER_LENGTH 20

void shiftLeft(char data[], int length);

int main (void) {

    char string[] = "                    Microcontrollers are lots of fun!";
    char banner[ BANNER_LENGTH + 1] = "";
    int count = 0;
    int i = 0;

	timer_init(); // Initialize Timer, needed before any LCD screen functions can be called 
	              // and enables time functions (e.g. timer_waitMillis)

	lcd_init();   // Initialize the LCD screen.  This also clears the screen. 

	// Print "Hello, world" on the LCD
	while(1){
        if (count < BANNER_LENGTH){
            banner[count] = string[i];
            count++;
            banner[count] = '\0';
        }
        else if (i >= strlen(string)){
            i = 0;
        }
        else {
            shiftLeft(banner, strlen(banner));
            banner[BANNER_LENGTH - 1] = string[i];
        }

        i++;

        lcd_printf("%s\n", banner);
        timer_waitMillis(300);
	}

	// lcd_puts("Hello, world"); // Replace lcd_printf with lcd_puts
        // step through in debug mode and explain to TA how it works
    
	// NOTE: It is recommended that you use only lcd_init(), lcd_printf(), lcd_putc, and lcd_puts from lcd.h.
       // NOTE: For time functions, see Timer.h

	return 0;
}

void shiftLeft (char data[], int n) {
    char firstChar = data[0];
    int j;
        for (j = 0; j < n - 1; j++) {
        data[j] = data[j + 1];
        }

        data[n - 1] = firstChar;
}
