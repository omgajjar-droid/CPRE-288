#include "open_interface.h"
#include "cyBot_uart.h"
#include "cyBot_Scan.h"
#include <stdio.h>
#include "movement.h"
#include "Timer.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

void cyBot_sendString(char *input) {
 while(*input != '\0') {
        cyBot_sendByte(*input);
        input++;
    }
}
float avgDistance(int i, cyBOT_Scan_t *scan_data) {
    float sum = 0;
   while(i<=6){
   cyBOT_Scan(i, scan_data);
   sum += scan_data -> sound_dist;
   i++;
   }
   return sum / 6;
}

    void detectObject(cyBOT_Scan_t *getScan){
        int angle = 0;
        int i =0;
        float smallestAngle = 0;
        float maxDist = 140;
        float smallestWidth = 99999;
        float width;
        float sum;
        int objCounter = 0;
        int objDetected = 0;
        cyBOT_Scan(0, getScan);
        float lastDistance =  avgDistance(0, getScan);
        float current;
        if(cyBot_getByte() == 'm') {

            while(angle <= 180){
                while(i <= 2){
                cyBOT_Scan(angle, getScan);
                current = getScan->sound_dist;
                sum += current;
                i++;
                }
                current = sum / 2.0f;
                sum = 0;
                i = 0;
                   if(objDetected == 1) {
                       //object done detecting
                       if(current > lastDistance + 10) {
                           objDetected = 0;
                           float centreAngle = (angle - (width / 2.0f));
                           char buffer[100];
                           //object number centre angle distance width
                           sprintf(buffer, "obj number: %d \r \t Angle: %f \t Distance: %f \t Width:%f \t\n", objCounter, centreAngle, lastDistance, width);
                           cyBot_sendString(buffer);
                           if(width < smallestWidth){
                               smallestWidth = width;
                               smallestAngle = centreAngle + 0.5;
                           }

                       }

                    width +=2;
                }
                   else if(current < lastDistance - 10 && current <= maxDist){
                       objDetected = 1;
                       objCounter ++;
                       width = 0;
                   }

            angle +=2;
            lastDistance = current;
        }

        }
        cyBOT_Scan(smallestAngle, getScan);
        return;
    }

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
    cyBot_uart_init();
    cyBOT_init_Scan(0b0011);
    right_calibration_value = 122500;
     left_calibration_value = 1477000;
//    cyBOT_SERVO_cal();
     cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));
     detectObject(getScan);
//     char buffer[100];
//
//    cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));

//    char buffer[100];
//    while(1){
//        char test = (char) cyBot_getByte();
//
//    sprintf(buffer, "Got an %c", test);
//    cyBot_sendString(buffer);
//    if(cyBot_getByte() == 'p'){
//        break;
//    }
//}
    return 0;
}

