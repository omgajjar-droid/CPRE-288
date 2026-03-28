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
float avgIR(int angle, cyBOT_Scan_t *scan_data) {
    float sum = 0;
    int samples = 5;

    for(int i = 0; i < samples; i++) {
        cyBOT_Scan(angle, scan_data);
        sum += scan_data->IR_raw_val;
    }

    return sum / samples;
}

void detectObject(cyBOT_Scan_t *scan) {
    int angle;

    int objStart = -1;
    int objEnd = -1;
    int objCount = 0;

    float threshold = 50;  

    char buffer[100];

    for(angle = 0; angle <= 180; angle += 2) {

        float ir = avgIR(angle, scan);

        if(ir < threshold && objStart == -1) {
            objStart = angle;
        }

        else if(ir >= threshold && objStart != -1) {
            objEnd = angle;

            int midAngle = (objStart + objEnd) / 2;

            
            cyBOT_Scan(midAngle, scan);
            float distance = scan->sound_dist;

        
            float angleWidth = (objEnd - objStart) * (3.14159 / 180.0);

            
            float width = 2 * distance * tan(angleWidth / 2);

            sprintf(buffer,
                "Obj %d | Angle: %d | Dist: %.2f | Width: %.2f\n",
                objCount, midAngle, distance, width);

            cyBot_sendString(buffer);

            objCount++;

            objStart = -1;
            objEnd = -1;
        }
    }
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

