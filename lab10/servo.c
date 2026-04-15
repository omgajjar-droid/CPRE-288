/*
 * servo.c
 *
 *  Created on: Apr 8, 2026
 *      Author: bmkunka
 */

#include "Timer.h"
#include "servo.h"

volatile uint32_t setting_0;
volatile uint32_t setting_180;
volatile uint8_t calibration = 0;

void servo_init(void){
   SYSCTL_RCGCGPIO_R |= 0x02; //Port B
   SYSCTL_RCGCTIMER_R |= 0x02; // Timer 1

   while((SYSCTL_PRGPIO_R & 0x02) != 0x02){}; // Checking if Port B is ready
   while((SYSCTL_PRTIMER_R & 0x02) != 0x02){}; // Checking if Timer 1 is ready

  // this is for setting up T1CCP1 on PB5
  GPIO_PORTB_AFSEL_R |= 0x20;
  GPIO_PORTB_PCTL_R  &= ~0x00F00000;
  GPIO_PORTB_PCTL_R |= 0x00700000;
  GPIO_PORTB_DEN_R |= 0x20;
  GPIO_PORTB_DIR_R |= 0x20;
  
  TIMER1_CTL_R &= ~0x100; // diable time
  TIMER1_CFG_R = 0x4; // setting 16 bits
  TIMER1_TBMR_R = 0x000A; // The PWC mode 

  // Set period to 320000 counts, which should go to 20ms at 16MHz
  uint32_t pwm_period = 320000;
  TIMER1_TBILR_R &= 0x0;
  TIMER1_TBILR_R = pwm_period & 0x0FFFF;
  TIMER1_TBPR_R &= 0x0;
  TIMER1_TBPR_R = pwm_period >> 16;
  TIMER1_TBMATCHR_R &= 0x0;
  TIMER1_TBPMR_R &= 0x0;

    TIMER1_CTL_R |= 0x100; // Enable time
}
// rest get the servo to move 
void servo_move(uint16_t degrees){
  long match_period;
  long match_shifted;
   
    if(!calibration){
        float match_value_ms = 20.0 - (degrees/180.0+1.0);
        match_period = (match_value_ms / 1000)/0.0000000625;
    }
    else{
        match_period = (((signed int)(setting_180 - setting_0))/180) * degrees + setting_0;
    }
   
    match_shifted = match_period >> 16;

    TIMER1_TBMATCHR_R = match_period & 0x0FFFF;
    TIMER1_TBPMR_R = match_shifted; 
}

// calibrate the servo
void servo_calibration(void){

   
}
  

