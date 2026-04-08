///**
// * Driver for ping sensor
// * @file ping.c
// * @author
// */
//
//#include "ping_template.h"
//#include "Timer.h"
//
//// Global shared variables
//// Use extern declarations in the header file
//
//volatile uint32_t g_start_time = 0;
//volatile uint32_t g_end_time = 0;
//volatile uint32_t g_pulse_width = 0;
//volatile uint32_t g_overflow_count = 0;
//volatile enum{LOW, HIGH, DONE} g_state = LOW; // State of ping echo pulse
//
//void ping_init (void){
//
//  // YOUR CODE HERE
//    SYSCTL_RCGCGPIO_R |= 0x02;
//    SYSCTL_RCGCTIMER_R |= 0x08;
//
//    while((SYSCTL_PRGPIO_R & 0x02) == 0){}
//    while((SYSCTL_PRTIMER_R & 0x08) == 0){}
//
//    GPIO_PORTB_DEN_R |= 0x08;
//    GPIO_PORTB_AFSEL_R |= 0x08;
//    GPIO_PORTB_PCTL_R &= ~0x0000F000;
//    GPIO_PORTB_PCTL_R |= 0x00007000;
//
//    TIMER3_CTL_R &= ~0x100;
//    TIMER3_CFG_R = 0x4;
//    TIMER3_TBMR_R = 0x07;
//    TIMER3_CTL_R |= 0x0C00;
//    TIMER3_TBPR_R = 0xFF;
//    TIMER3_TBILR_R = 0xFFFF;
//    TIMER3_ICR_R |= 0x400;
//    TIMER3_IMR_R |= 0x400;
//
//    NVIC_EN1_R |= 0x10;
//    NVIC_PRI9_R = (NVIC_PRI9_R & 0xFFFFFF0F) | 0x00000020;
//
//    IntRegister(INT_TIMER3B, TIMER3B_Handler);
//
//    IntMasterEnable();
//
//    // Configure and enable the timer
//    TIMER3_CTL_R |= 0x100;
//}
//
//void ping_trigger (void){
//    g_state = LOW;
//    // Disable timer and disable timer interrupt
//    TIMER3_CTL_R &= ~0x100;
//    TIMER3_IMR_R &= ~0x400;
//    // Disable alternate function (disconnect timer from port pin)
//    GPIO_PORTB_AFSEL_R &= ~0x08;
//
//    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
//    GPIO_PORTB_DIR_R |= 0x08;
//    GPIO_PORTB_DATA_R &= ~0x08;
//    timer_waitMicros(2);
//    GPIO_PORTB_DATA_R |= 0x08;
//    timer_waitMicros(5);
//    GPIO_PORTB_DATA_R &= ~0x08;
//
//    // Clear an interrupt that may have been erroneously triggered
//    TIMER3_ICR_R |= 0x400;
//    // Re-enable alternate function, timer interrupt, and timer
//    GPIO_PORTB_AFSEL_R |= 0x08;
//    TIMER3_IMR_R |= 0x400;
//    TIMER3_CTL_R |= 0x100;
//}
//
//void TIMER3B_Handler(void){
//
//  // YOUR CODE HERE
//
//    if(TIMER3_MIS_R & 0x400){
//        TIMER3_ICR_R |= 0x400;
//
//        if(g_state == LOW){
//            g_start_time = TIMER3_TBR_R;
//            g_state = HIGH;
//        }
//        else if(g_state == HIGH){
//            g_end_time = TIMER3_TBR_R;
//            g_state = DONE;
//        }
//    }
//}
//
//float ping_getDistance (void){
//
//    // YOUR CODE HERE
//    uint32_t pulse_width;
//    float time_us;
//    float distance_cm;
//
//    ping_trigger();
//
//    while(g_state != DONE){
//    }
//
//    if(g_start_time >= g_end_time){
//        pulse_width = g_start_time - g_end_time;
//    }
//    else{
//        pulse_width = g_start_time + (0xFFFFFF - g_end_time) + 1;
//        g_overflow_count++;
//    }
//
//    g_pulse_width = pulse_width;
//
//    time_us = pulse_width / 16.0;
//    distance_cm = time_us / 58.0;
//
//    return distance_cm;
//}
//float getpulseWidth(void){
//    return (float)g_pulse_width;
//}
//
//float getOverflow(void){
//    return (float)g_overflow_count;
//}
