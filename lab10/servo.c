/*
 * servo.c
 *
 *  Created on: Apr 8, 2026
 *      Author: bmkunka
 */
#include "Timer.h"
#include "servo.h"
#include "lcd.h"
#include "button.h"

#define COUNTS_0        16000
#define COUNTS_180      32000
#define COUNTS_PER_DEG  ((float)(COUNTS_180 - COUNTS_0) / 180.0f)

volatile uint32_t setting_0 = COUNTS_0;
volatile uint32_t setting_180 = COUNTS_180;
volatile uint8_t calibration = 0;

void servo_init(void){
    SYSCTL_RCGCTIMER_R |= 0x2; // enable port B timer
    while ((SYSCTL_PRTIMER_R & 0x2) == 0) {} // wait for port B timer to be ready
    TIMER1_CTL_R &= ~0x100; // diable time
    TIMER1_CTL_R |= 0x4000; // invert pwm out signal so that match is low and rest of period is high
    TIMER1_CFG_R |= 0x4; // setting 16 bits
    TIMER1_TBMR_R |= 0b1010; // The PWC mode
    TIMER1_TBPR_R  |= 0x4; // extends TBILR timer with extra prescaler register 3 bits
    TIMER1_TBILR_R |= 0xE200; // Set period to 320000 counts, which should go to 20ms at 16MHz
    TIMER1_TBPMR_R &= 0x00; // set prescale match register to 0x00 so no default to 0xFF
    // initial position 90 deg
    uint32_t start = COUNTS_0 + (uint32_t)(90 * COUNTS_PER_DEG);
    TIMER1_TBMATCHR_R = start & 0xFFFF;
    TIMER1_TBPMR_R    = (start >> 16) & 0xFF;
    // init PB5
    SYSCTL_RCGCGPIO_R |= 0x2;
    while((SYSCTL_PRGPIO_R & 0x2) == 0) {}
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R  |= 0x700000;
    GPIO_PORTB_DEN_R   |= 0x20;
    GPIO_PORTB_DIR_R   |= 0x20;
    TIMER1_CTL_R |= 0x100; // Enable time
}

// rest get the servo to move
void servo_move(uint16_t degrees){
    if(degrees > 180) degrees = 180;
    uint32_t match;
    if(!calibration){
        match = COUNTS_0 + (uint32_t)(degrees * COUNTS_PER_DEG);
    }
    else{
        match = setting_0 + (uint32_t)(degrees * ((float)(setting_180 - setting_0) / 180.0f));
    }
    TIMER1_TBMATCHR_R = match & 0xFFFF;
    TIMER1_TBPMR_R    = (match >> 16) & 0xFF;
    timer_waitMillis(500);
}

// calibrate the servo
void servo_calibration(void){
    char buffer[90];
    uint8_t button = 0;

    // Step 1: set 0 degrees
    servo_move(0);
    lcd_printf("Nudge to 0 deg\nB4 to confirm");
    timer_waitMillis(1000);

    while(button != 4){
        uint32_t match = ((TIMER1_TBPMR_R & 0xFF) << 16)
                        | (TIMER1_TBMATCHR_R & 0xFFFF);
        button = button_getButton();
        switch(button){
            case 1:
                match += 10;
                timer_waitMillis(200);
                break;
            case 2:
                match -= 10;
                timer_waitMillis(200);
                break;
            case 3:
                match += 100;
                timer_waitMillis(200);
                break;
        }
        TIMER1_TBMATCHR_R = match & 0xFFFF;
        TIMER1_TBPMR_R    = (match >> 16) & 0xFF;
        sprintf(buffer, "%u | Set 0", match);
        lcd_printf(buffer);
    }
    setting_0 = ((TIMER1_TBPMR_R & 0xFF) << 16) | (TIMER1_TBMATCHR_R & 0xFFFF);
    lcd_printf("0 deg locked in!");
    timer_waitMillis(1000);

    // Step 2: set 180 degrees
    servo_move(180);
    lcd_printf("Nudge to 180 deg\nB4 to confirm");
    timer_waitMillis(1000);

    button = 0;
    while(button != 4){
        uint32_t match = ((TIMER1_TBPMR_R & 0xFF) << 16)
                        | (TIMER1_TBMATCHR_R & 0xFFFF);
        button = button_getButton();
        switch(button){
            case 1:
                match += 10;
                timer_waitMillis(200);
                break;
            case 2:
                match -= 10;
                timer_waitMillis(200);
                break;
            case 3:
                match += 100;
                timer_waitMillis(200);
                break;
        }
        TIMER1_TBMATCHR_R = match & 0xFFFF;
        TIMER1_TBPMR_R    = (match >> 16) & 0xFF;
        sprintf(buffer, "%u | Set 180", match);
        lcd_printf(buffer);
    }
    setting_180 = ((TIMER1_TBPMR_R & 0xFF) << 16) | (TIMER1_TBMATCHR_R & 0xFFFF);
    lcd_printf("180 deg locked!");
    timer_waitMillis(1000);

    calibration = 1;
    servo_move(90);
}
