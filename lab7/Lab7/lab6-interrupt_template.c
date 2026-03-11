///**
// * lab6-interrupt_template.c
// *
// * Template file for CprE 288 Lab 6
// *
// * @author Diane Rover, 2/15/2020
// *
// */
//
#include "open_interface.h"
#include "cyBot_Scan.h"
#include <stdio.h>
#include "movement.h"
#include "Timer.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "uart-interrupt.h"

#define REPLACEME 0

void detectObject(void){

}
float avgDistance_IR(int i, cyBOT_Scan_t *scan_data) {
    float sum = 0;
   while(i<=6){
   cyBOT_Scan(i, scan_data);
   sum += scan_data -> IR_raw_val;
   i++;
   }
   return sum / 6;
}

float avgDistance_Ping(int i, cyBOT_Scan_t *scan_data) {
    float sum = 0;
   while(i<=6){
   cyBOT_Scan(i, scan_data);
   sum += scan_data -> sound_dist;
   i++;
   }
   return sum / 6;
}


//int main(void) {
//	timer_init();
//	lcd_init();
//	uart_interrupt_init();
//
//	command_byte = 'm';
//
//	uart_sendStr("UART ready\r\n");
//	while(1)
//	{
//
//	    if(command_flag == 1){
//	        lcd_printf("M detected");
//	        uart_sendStr(" receieved\r\n");
//	        command_flag = 0;
//	    }
//
//	}
//
//}
//




//int i;
//bool objDetected = false;
//int width;
//int widthEnd;
//int avg;
//while(i <= 180) {
//    if(cyBot_getByte() == 'm') {
//    cyBOT_Scan(i, getScan);
//
//
//        sprintf(buffer, "%d \r %f \t \n" , i, getScan->sound_dist);
//        cyBot_sendString(buffer);
//        printf(buffer);
//
//        if (getScan->sound_dist < 30){
//            objDetected = true;
//            width = i;
//
//            if (getScan->sound_dist > 30){
//                widthEnd = i;
//            }
//        }
//        avg = widthEnd - width;
//        //objValue(avg);
//
//}
//
//}
int main(void) {
    timer_init();
    lcd_init();
    cyBOT_init_Scan(0b0011);
    right_calibration_value = 122500;
     left_calibration_value = 1477000;
    cyBOT_SERVO_cal();
//     cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));
//     detectObject(getScan);
////     char buffer[100];
////
////    cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));
//
////    char buffer[100];
////    while(1){
////        char test = (char) cyBot_getByte();
////
////    sprintf(buffer, "Got an %c", test);
////    cyBot_sendString(buffer);
////    if(cyBot_getByte() == 'p'){
////        break;
////    }
////}
    return 0;
}
