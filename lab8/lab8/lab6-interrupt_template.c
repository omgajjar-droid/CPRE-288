#include "open_interface.h"
#include "cyBot_Scan.h"
#include <stdio.h>
#include "movement.h"
#include "Timer.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "uart-interrupt.h"
#include "adc.h"

#define MAX_OBJECTS 10

#define IR_ENTER_THRESHOLD 800.0f
//#define IR_EXIT_THRESHOLD  600.0f

#define MIN_WIDTH  2.0f
#define MAX_WIDTH  35.0f

#define MIN_ANGLE_SPAN 2

#define MIN_DIST  5.0f
#define MAX_DIST  200.0f

typedef struct {
    int   startAngle;
    int   endAngle;
    int   midAngle;
    float distance;
    float width;
} Object;

float avgIR(int angle, cyBOT_Scan_t *scan_data) {
    float sum = 0;
    int samples = 2;
    int i;
    cyBOT_Scan(angle, scan_data);
    for (i = 0; i < samples; i++) {
        sum += adc_read();
    }
    return sum / samples;
}

float getReliableDist(int midAngle, cyBOT_Scan_t *scan) {
//    float a = 25.0f;
//    float b = 0.2f;
//    float c = 0.4f;
//
//    float sum = 0;
//    int validCount = 0;
//    float distToCm;
    float dist;
    int i;

    for (i = 0; i < 5; i++) {
        cyBOT_Scan(midAngle, scan);

         dist = scan->sound_dist;

//        if ((V - b) <= 0) continue;

//        float dist = (rawDist*10) / 2;
//
//        distToCm = dist / 2257;

//        if (dist >= 9.0f && dist <= 80.0f) {
//            sum += dist;
//            validCount++;
//        }
    }

//    if (validCount == 0) return -1.0f;
    return dist;
}

void detectObject(cyBOT_Scan_t *scan, Object *objects, int *objCount, int *smallestIndex, char *buffer) {

    int angle;
    int objStart = -1;
    int objEnd   = -1;
    float ir;
    float smallestWidth = 9999;

    *objCount      = 0;
    *smallestIndex = -1;

    uart_sendStr("Scanning...\r\n");

    for (angle = 0; angle <= 180; angle += 2) {

        if (command_flag == 2) {
            uart_sendStr("Scanning stopped\r\n");
            break;
        }

        ir = avgIR(angle, scan);

        snprintf(buffer, 100, "Angle: %d | IR: %.2f\r\n", angle, ir);
        uart_sendStr(buffer);

        if (ir > IR_ENTER_THRESHOLD) {
            if (objStart == -1) {
                objStart = angle;
            }
        } else if (ir < IR_ENTER_THRESHOLD) {
            if (objStart != -1) {

                objEnd = angle - 2;

                int midAngle = (objStart + objEnd) / 2;
                cyBOT_Scan(midAngle, scan);

                float distance = getReliableDist(midAngle, scan);

                if (distance < 0) {
                    snprintf(buffer, 100,
                             "Candidate REJECTED (bad sonar) | Start: %d | End: %d\r\n",
                             objStart, objEnd);
                    uart_sendStr(buffer);
                } else {
                    float angleRad = (objEnd - objStart) * (3.14159f / 180.0f);
                    float width    = 2.0f * distance * tan(angleRad / 2.0f);

                    snprintf(buffer, 100,
                             "Candidate | Start: %d | End: %d | Mid: %d | Dist: %.2f | Width: %.2f\r\n",
                             objStart, objEnd, midAngle, distance, width);
                    uart_sendStr(buffer);

                    if ((objEnd - objStart) >= MIN_ANGLE_SPAN
                            && width >= MIN_WIDTH
                            && width <= MAX_WIDTH
                            && *objCount < MAX_OBJECTS) {

                        Object *obj   = &objects[*objCount];
                        obj->startAngle = objStart;
                        obj->endAngle   = objEnd;
                        obj->midAngle   = midAngle;
                        obj->distance   = distance;
                        obj->width      = width;

                        snprintf(buffer, 100,
                                 "Obj %d | Angle: %d | Dist: %.2f | Width: %.2f\r\n",
                                 *objCount, midAngle, distance, width);
                        uart_sendStr(buffer);

                        if (width < smallestWidth) {
                            smallestWidth  = width;
                            *smallestIndex = *objCount;
                        }

                        (*objCount)++;
                    }
                }

                objStart = -1;
                objEnd   = -1;
            }
        }

    }

    if (objStart != -1) {
        objEnd = 180;
        int midAngle  = (objStart + objEnd) / 2;
        float distance = getReliableDist(midAngle, scan);

        if (distance > 0) {
            float angleRad = (objEnd - objStart) * (3.14159f / 180.0f);
            float width    = 2.0f * distance * tan(angleRad / 2.0f);

            snprintf(buffer, 100,
                     "Edge Candidate | Mid: %d | Dist: %.2f | Width: %.2f\r\n",
                     midAngle, distance, width);
            uart_sendStr(buffer);

            if ((objEnd - objStart) >= MIN_ANGLE_SPAN
                    && width >= MIN_WIDTH
                    && width <= MAX_WIDTH
                    && *objCount < MAX_OBJECTS) {

                Object *obj   = &objects[*objCount];
                obj->startAngle = objStart;
                obj->endAngle   = objEnd;
                obj->midAngle   = midAngle;
                obj->distance   = distance;
                obj->width      = width;

                if (width < smallestWidth) {
                    smallestWidth  = width;
                    *smallestIndex = *objCount;
                }
                (*objCount)++;
            }
        }
    }

    // Report result
    if (*smallestIndex == -1) {
        uart_sendStr("No valid objects detected\r\n");
    } else {
        snprintf(buffer, 100,
                 "Smallest: Obj %d | Angle: %d | Dist: %.2f | Width: %.2f\r\n",
                 *smallestIndex,
                 objects[*smallestIndex].midAngle,
                 objects[*smallestIndex].distance,
                 objects[*smallestIndex].width);
        uart_sendStr(buffer);
        lcd_printf("%d deg\n%.2fcm",
                   objects[*smallestIndex].midAngle,
                   objects[*smallestIndex].distance);
    }
   return;
}

//void findObject(cyBOT_Scan_t *getscan, Object *objects, int *smallestIndex, char *buffer){
//    oi_t *roomba = oi_alloc();
//    oi_init(roomba);
//    char status;
//
//    if (*smallestIndex != -1) {
//                    int targetAngle    = objects[smallestIndex].midAngle;
//                    float targetDist   = objects[smallestIndex].distance;
//
//                    double driveDistance = (targetDist * 10.0);
//
//                    snprintf(buffer, 100, "Turning to %d deg, driving %.1f mm\r\n",
//                    targetAngle, driveDistance);
//                    uart_sendStr(buffer);
//                    cyBOT_Scan(targetAngle, getScan);
//                    timer_waitMillis(1000);
//
//                    if (targetAngle < 90) {
//                        // Object is to the right
//                        turn_Clockwise(roomba, 90 - targetAngle); // original: 90 - targetAngle + 20
//                    } else if (targetAngle > 90) {
//                        // Object is to the left
//                        turn_CntrClockwise(roomba, targetAngle - 90); //original: targetAngle - 110
//                    }
//                    timer_waitMillis(500);
//
//                    if (driveDistance > 0) {
//                        move_forward(roomba, driveDistance, 150);
//
//                        if (status == 'l') {
//                            oi_setWheels(0, 0);
//                            lcd_printf("BumpLeft!");
//                            timer_waitMillis(1000);
//                            move_backward(sensor_data,150,100);
//                            turn_Clockwise(sensor_data,70);
//
//                        }
//                        else if (status == 'r') {
//                            oi_setWheels(0, 0);
//                            lcd_printf("BumpRight!");
//                            timer_waitMillis(1000);
//                            move_backward(sensor_data,150,100);
//                            turn_CntrClockwise(sensor_data,70);
//                        }
//                    }
//    }
//                   return;
//}

int main(void) {
    timer_init();
    lcd_init();
    adc_init();

    oi_t *roomba = oi_alloc();
    oi_init(roomba);

    cyBOT_init_Scan(0b0111);
    uart_interrupt_init();
//    cyBOT_SERVO_cal();

    right_calibration_value = 243250;
    left_calibration_value  = 1251250;

    command_byte = 'm';

    cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));

    Object objects[MAX_OBJECTS];
    int   objCount = 0;
    int   smallestIndex = -1;
    char  buffer[100];
//    char status;

    cyBOT_Scan(0, getScan);

    while (1) {
        if (command_flag == 1) {

            detectObject(getScan, objects, &objCount, &smallestIndex, buffer);
//            findObject(getScan, objects, &smallestIndex, buffer);

            if (smallestIndex != -1) {
                int targetAngle    = objects[smallestIndex].midAngle;
                float targetDist   = objects[smallestIndex].distance;

                double driveDistance = (targetDist * 10.0);

                snprintf(buffer, 100, "Turning to %d deg, driving %.1f mm\r\n", targetAngle, driveDistance);
                uart_sendStr(buffer);
                cyBOT_Scan(targetAngle, getScan);
                timer_waitMillis(1000);

                if (targetAngle < 90) {
                    // Object is to the right
                    turn_Clockwise(roomba, 90 - targetAngle); // original: 90 - targetAngle + 20
                } else if (targetAngle > 90) {
                    // Object is to the left
                    turn_CntrClockwise(roomba, targetAngle - 90); //original: targetAngle - 110
                }
                timer_waitMillis(500);

                if (driveDistance > 0) {
                    move_forward(roomba, driveDistance, 150);

//                    if (status == 'l') {
//                        oi_setWheels(0, 0);
//                        lcd_printf("BumpLeft!");
//                        timer_waitMillis(1000);
//                        move_backward(sensor_data,150,100);
//                        turn_Clockwise(sensor_data,70);
//
//                    }
//                    else if (status == 'r') {
//                        oi_setWheels(0, 0);
//                        lcd_printf("BumpRight!");
//                        timer_waitMillis(1000);u
//                        move_backward(sensor_data,150,100);
//                        turn_CntrClockwise(sensor_data,70);
//                    }


                }
//                detectObject(getScan, objects, &objCount, &smallestIndex, buffer);
//                if (driveDistance > 0) {
//                    move_forward(roomba, 30, 150);
//                }

                uart_sendStr("Reached target!\r\n");
                lcd_printf("Done!\n%d deg", targetAngle);
            }

            command_flag = 0;
        }
    }
}
