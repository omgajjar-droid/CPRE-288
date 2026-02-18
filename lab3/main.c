#include "open_interface.h"
#include "cyBot_uart.h"
#include "cyBot_Scan.h"
#include <stdio.h>
#include "movement.h"
#include "Timer.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdbool.h>

void cyBot_sendString(char *input) {
 while(*input != '\0') {
        cyBot_sendByte(*input);
        input++;
    }
}

//    void objectFinder(int objValue){
//        int i;
//        while
//        it j;
//        int objectList[];
//        int arrLength = sizeof(objectList);
        //for (i = 0; i < arrLength; i++){
            //for (j = i+1; j <arrLength; j++){
//        }
//
//        }
//
//    }


    void detectObject(){
        char buffer[100];

       cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));
        int i = 0;
//        int objDetected = 0;
        int width =0;
        int widthEnd =0;
        int avg = 0;
        while(i <= 180) {
            if(cyBot_getByte() == 'm') {
            cyBOT_Scan(i, getScan);


                sprintf(buffer, "%d \r %f \t \n" , i, getScan->sound_dist);
                cyBot_sendString(buffer);


                if (getScan->sound_dist < 30){
//                    objDetected = 1;
                    width = i;
                    if (getScan->sound_dist > 30){
                        widthEnd = i;
                        avg = widthEnd - width;
                    }
                }
                //objValue(avg);

        }
            i +=2;
        }

        lcd_printf("%d", avg);
        cyBot_sendString("Object was d");
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
    right_calibration_value = 169750;
     left_calibration_value = 1309000;
//     cyBOT_SERVO_cal();
     detectObject();
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

