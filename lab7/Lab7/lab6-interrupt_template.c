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

float avgIR(int angle, cyBOT_Scan_t *scan_data) {
    float sum = 0;
    int samples = 5;
    int i;
    for(i = 0; i < samples; i++) {
        cyBOT_Scan(i, scan_data);
        sum += scan_data->IR_raw_val;
    }

    return sum / samples;
}

void detectObject(cyBOT_Scan_t *scan) {
    int angle = 0;

    int objStart = -1;
    int objEnd = -1;
    int objCount = 0;
    float threshold = 50;  
    float ir;
    char buffer[100];
    cyBOT_Scan(0, scan);
    uart_sendStr("Scanning...\r\n");
    while(angle <= 180) {

            cyBOT_Scan(angle, scan);

            ir = avgIR(angle, scan);

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

            sprintf(buffer,"Obj %d | Angle: %d | Dist: %.2f | Width: %.2f\r\n", objCount, midAngle, distance, width);

            uart_sendStr(buffer);

            objCount++;

            objStart = -1;
            objEnd = -1;
        }
        sprintf(buffer, "Angle: %d IR: %.2f\r\n", angle, ir);
        uart_sendStr(buffer);
        angle += 2;
    }

}



int main(void) {
    timer_init();
    lcd_init();
    cyBOT_init_Scan(0b0111);
    uart_interrupt_init();
    right_calibration_value = -271250;
    left_calibration_value = 1477000;
    command_byte = 'm';
//    cyBOT_SERVO_cal();
     cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));
     while(1){
         if(command_flag == 1){
             detectObject(getScan);
             command_flag = 0;
         }
         if(command_flag == 2){
             return 0;
         }
     }


}

