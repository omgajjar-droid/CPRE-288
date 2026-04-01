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

/*
 * Move servo to the requested angle, then take multiple ADC samples
 * and average them for a more stable IR reading.
 */
float avgADC(int angle, cyBOT_Scan_t *scan_data) {
    int i;
    float sum = 0.0f;
    int samples = 16;

    // Move servo / update scan position
    cyBOT_Scan(angle, scan_data);
    timer_waitMillis(25);

    for (i = 0; i < samples; i++) {
        sum += adc_read();
        timer_waitMillis(2);
    }

    return sum / samples;
}

/*
 * Use the PING sensor distance at the object's midpoint.
 * Take several readings and average them.
 */
float getReliableDist(int midAngle, cyBOT_Scan_t *scan) {
    int i;
    float sum = 0.0f;
    int validCount = 0;

    for (i = 0; i < 5; i++) {
        cyBOT_Scan(midAngle, scan);
        timer_waitMillis(20);

        if (scan->sound_dist >= MIN_DIST && scan->sound_dist <= MAX_DIST) {
            sum += scan->sound_dist;
            validCount++;
        }
    }

    if (validCount == 0) {
        return -1.0f;
    }

    return sum / validCount;
}

void detectObject(cyBOT_Scan_t *scan, Object *objects, int *objCount, int *smallestIndex, char *buffer) {
    int angle;
    int objStart = -1;
    int objEnd   = -1;
    float ir;
    float smallestWidth = 9999.0f;

    *objCount      = 0;
    *smallestIndex = -1;

    uart_sendStr("Scanning...\r\n");

    for (angle = 0; angle <= 180; angle += 2) {

        if (command_flag == 2) {
            uart_sendStr("Scanning stopped\r\n");
            break;
        }

        ir = avgADC(angle, scan);

        snprintf(buffer, 100, "Angle: %d | ADC IR: %.2f\r\n", angle, ir);
        uart_sendStr(buffer);

        if (ir > IR_ENTER_THRESHOLD) {
            if (objStart == -1) {
                objStart = angle;
            }
        }
        else {
            if (objStart != -1) {
                objEnd = angle - 2;

                int midAngle = (objStart + objEnd) / 2;
                float distance = getReliableDist(midAngle, scan);

                if (distance < 0) {
                    snprintf(buffer, 100,
                             "Candidate REJECTED (bad sonar) | Start: %d | End: %d\r\n",
                             objStart, objEnd);
                    uart_sendStr(buffer);
                }
                else {
                    float angleRad = (objEnd - objStart) * (3.14159f / 180.0f);
                    float width = 2.0f * distance * tanf(angleRad / 2.0f);

                    snprintf(buffer, 100,
                             "Candidate | Start: %d | End: %d | Mid: %d | Dist: %.2f | Width: %.2f\r\n",
                             objStart, objEnd, midAngle, distance, width);
                    uart_sendStr(buffer);

                    if ((objEnd - objStart) >= MIN_ANGLE_SPAN &&
                        width >= MIN_WIDTH &&
                        width <= MAX_WIDTH &&
                        *objCount < MAX_OBJECTS) {

                        Object *obj = &objects[*objCount];
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

    // Handle object that continues through 180 degrees
    if (objStart != -1) {
        objEnd = 180;
        int midAngle = (objStart + objEnd) / 2;
        float distance = getReliableDist(midAngle, scan);

        if (distance > 0) {
            float angleRad = (objEnd - objStart) * (3.14159f / 180.0f);
            float width = 2.0f * distance * tanf(angleRad / 2.0f);

            snprintf(buffer, 100,
                     "Edge Candidate | Mid: %d | Dist: %.2f | Width: %.2f\r\n",
                     midAngle, distance, width);
            uart_sendStr(buffer);

            if ((objEnd - objStart) >= MIN_ANGLE_SPAN &&
                width >= MIN_WIDTH &&
                width <= MAX_WIDTH &&
                *objCount < MAX_OBJECTS) {

                Object *obj = &objects[*objCount];
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

    if (*smallestIndex == -1) {
        uart_sendStr("No valid objects detected\r\n");
        lcd_printf("No objects");
    } else {
        snprintf(buffer, 100,
                 "Smallest: Obj %d | Angle: %d | Dist: %.2f | Width: %.2f\r\n",
                 *smallestIndex,
                 objects[*smallestIndex].midAngle,
                 objects[*smallestIndex].distance,
                 objects[*smallestIndex].width);
        uart_sendStr(buffer);

        lcd_printf("%d deg\n%.2f cm",
                   objects[*smallestIndex].midAngle,
                   objects[*smallestIndex].distance);
    }
}

int main(void) {
    timer_init();
    lcd_init();
    adc_init();
    uart_interrupt_init();

    oi_t *roomba = oi_alloc();
    oi_init(roomba);

    cyBOT_init_Scan(0b0111);

    right_calibration_value = 243250;
    left_calibration_value  = 1251250;

    command_byte = 'm';

    cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));

    Object objects[MAX_OBJECTS];
    int objCount = 0;
    int smallestIndex = -1;
    char buffer[100];

    cyBOT_Scan(0, getScan);

    while (1) {
        if (command_flag == 1) {

            detectObject(getScan, objects, &objCount, &smallestIndex, buffer);

            if (smallestIndex != -1) {
                int targetAngle = objects[smallestIndex].midAngle;
                float targetDist = objects[smallestIndex].distance;

                double driveDistance = targetDist * 10.0;

                snprintf(buffer, 100,
                         "Turning to %d deg, driving %.1f mm\r\n",
                         targetAngle, driveDistance);
                uart_sendStr(buffer);

                cyBOT_Scan(targetAngle, getScan);
                timer_waitMillis(1000);

                if (targetAngle < 90) {
                    turn_Clockwise(roomba, 90 - targetAngle);
                }
                else if (targetAngle > 90) {
                    turn_CntrClockwise(roomba, targetAngle - 90);
                }

                timer_waitMillis(500);

                if (driveDistance > 0) {
                    move_forward(roomba, driveDistance, 150);
                }

                uart_sendStr("Reached target!\r\n");
                lcd_printf("Done!\n%d deg", targetAngle);
            }

            command_flag = 0;
        }
    }
}