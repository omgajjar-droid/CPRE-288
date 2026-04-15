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
   SYSCTL_RCGCGPIO_R |= 0x2; //Port B
   SYSCTL_RCGCTIMER_R |= 0X02; // Timer 1

   while((SYSCTL_PRGPIO_R & 0x02) != 0x02){}; // Checking if Port B is ready
   while((SYSCTL_PRTIMER_R & 0x08) != 0x02){}; // Checking if Timer 1 is ready

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
  TIMER1_TBILR_R = pwm_period & 0xFFFF;
  TIMER1_TBPR_R  = pwm_period >> 16;

  // Set initial position to 90 degrees when equaling 296000
  uint32_t init_match = 296000;
  TIMER1_TBMATCHR_R = init_match & 0xFFFF;
  TIMER1_TBPMR_R    = init_match >> 16;

    TIMER1_CTL_R |= 0x100; // Enable time
}
// rest get the servo to move 
void servo_move(uint16_t degrees){
  
}
// this is for calibrating the move, and also makes it so that the button can set the angle
// this is mainly for parts 2 and 3, I think
void servo_calibration(void){
  
}

