/**
 * @file lab9_template.c
 * @author
 * Template file for CprE 288 Lab 9
 */

#include "Timer.h"
#include "lcd.h"
#include "ping_template.h"
#include "servo.h"

// Uncomment or add any include directives that are needed


#define REPLACEME 0

int main(void) {
    timer_init();
    lcd_init();
    ping_init();
    button_init();
    servo_init();

    // YOUR CODE HERE
   /* float distance = 0;
    float pulse_width = 0;
    float pulse_time_ms = 0;
    float overflow_count = 0; 
    */

    while(1)
    {

      // YOUR CODE HERE this is only for part 1
        servo_move(90);
        lcd_printf("servo at 90 degrees");
        timer_waitMillis(1500);
        servo_move(30);
        lcd_printf("servo at 30 degrees");
        timer_waitMillis(1500);
        servo_move(150);
        lcd_printf("servo at 150 degrees");
        timer_waitMillis(1500);
        servo_move(90);
        lcd_printf("servo at 90 degrees");
        timer_waitMillis(1500);

    // there is a button use for part 2

    }

}
