/**
 * @file lab9_template.c
 * @author
 * Template file for CprE 288 Lab 9
 */

#include "Timer.h"
#include "lcd.h"
#include "ping_template.h"

// Uncomment or add any include directives that are needed


#define REPLACEME 0

int main(void) {
    timer_init();
    lcd_init();
    ping_init();

    // YOUR CODE HERE
    float distance = 0;
    float pulse_width = 0;
    float pulse_time_ms = 0;
    float overflow_count = 0;

    while(1)
    {

      // YOUR CODE HERE
        distance = ping_getDistance();
        pulse_width = getpulseWidth();
        overflow_count = getOverflow();


        pulse_time_ms = pulse_width / 1000;

        lcd_printf("PW:%0.0f cyc\nT:%0.2f ms\nD:%0.2f cm\nOF:%0.0f",
                   pulse_width, pulse_width_ms, distance, overflow_count);

        timer_waitMillis(200);

    }

}
