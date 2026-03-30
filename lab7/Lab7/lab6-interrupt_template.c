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

// -------------------------------------------------------
// CONSTANTS
// -------------------------------------------------------
#define MAX_OBJECTS 10

// IR thresholds tuned from real scan data.
// enter_threshold: IR must exceed this to BEGIN an object.
// exit_threshold:  IR must DROP below this to END an object.
// The gap between them (hysteresis) prevents noisy ramp-ups
// from fragmenting or inflating object detections.
#define IR_ENTER_THRESHOLD 900.0f
#define IR_EXIT_THRESHOLD  600.0f

// Linear width limits in cm. Anything outside this range
// is rejected — walls are too wide, noise spikes too narrow.
#define MIN_WIDTH  5.0f
#define MAX_WIDTH  35.0f

// An object must span at least this many degrees.
// Kills single-angle noise spikes that produce 0-width candidates.
#define MIN_ANGLE_SPAN 6

// Sonar sanity bounds in cm.
// Readings outside this range are treated as bad pings.
#define MIN_DIST  5.0f
#define MAX_DIST  200.0f

// -------------------------------------------------------
// OBJECT STRUCT
// -------------------------------------------------------
typedef struct {
    int   startAngle;
    int   endAngle;
    int   midAngle;
    float distance;
    float width;
} Object;

// -------------------------------------------------------
// avgIR
// Averages multiple IR readings at a given angle to reduce noise.
// More samples = smoother but slower. 10 is a good balance.
// -------------------------------------------------------
float avgIR(int angle, cyBOT_Scan_t *scan_data) {
    float sum = 0;
    int samples = 10;
    int i;
    for (i = 0; i < samples; i++) {
        cyBOT_Scan(angle, scan_data);
        sum += scan_data->IR_raw_val;
    }
    return sum / samples;
}

// -------------------------------------------------------
// getReliableDist
// Takes multiple sonar readings at midAngle, discards any
// that fall outside plausible room range, and returns the
// average of the valid ones. Returns -1 if all readings fail.
// This prevents a single bad sonar ping from producing a
// wildly incorrect width calculation.
// -------------------------------------------------------
float getReliableDist(int midAngle, cyBOT_Scan_t *scan) {
    // IR distance formula: dist = a / (V - b) - c
    // where V = raw ADC value * (3.3 / 4095)
    // These constants are bot/sensor specific — recalibrate if readings seem off
    float a = 25.0f;
    float b = 0.2f;
    float c = 0.4f;

    float sum = 0;
    int validCount = 0;
    int i;

    for (i = 0; i < 5; i++) {
        cyBOT_Scan(midAngle, scan);
        float V = scan->IR_raw_val * (3.3f / 4095.0f);

        // Avoid divide by zero or negative denominator
        if ((V - b) <= 0) continue;

        float dist = (a / (V - b)) - c;

        // Only accept readings in the IR sensor's reliable range (9-80cm)
        if (dist >= 9.0f && dist <= 80.0f) {
            sum += dist;
            validCount++;
        }
    }

    if (validCount == 0) return -1.0f;
    return sum / validCount;
}

// -------------------------------------------------------
// detectObject
// Scans 0-180 degrees using IR for edge detection and sonar
// for distance. Populates the objects array and sets
// smallestIndex to the index of the narrowest valid object.
// -------------------------------------------------------
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

        // Allow scan to be aborted via UART command
        if (command_flag == 2) {
            uart_sendStr("Scanning stopped\r\n");
            break;
        }

        ir = avgIR(angle, scan);

        // Per-angle debug print. Comment out once tuning is done.
        snprintf(buffer, 100, "Angle: %d | IR: %.2f\r\n", angle, ir);
        uart_sendStr(buffer);

        // --- Hysteresis edge detection ---
        // Using two thresholds prevents a gradual ramp-up at the
        // edge of an object from inflating its angular span.
        if (ir > IR_ENTER_THRESHOLD) {
            if (objStart == -1) {
                objStart = angle;
            }
        } else if (ir < IR_EXIT_THRESHOLD) {
            if (objStart != -1) {

                // FIX: objEnd is the last angle ABOVE threshold,
                // not the first angle below it (off-by-one).
                objEnd = angle - 2;

                int midAngle = (objStart + objEnd) / 2;

                // Get a reliable sonar distance
                float distance = getReliableDist(midAngle, scan);

                // Print every candidate before filtering so you
                // can see what is being rejected and why.
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

                    // Apply filters:
                    // 1. Must span at least MIN_ANGLE_SPAN degrees (kills noise spikes)
                    // 2. Width must be within plausible object range (kills walls and noise)
                    // 3. Must have room in the objects array
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
        // If IR is between the two thresholds, do nothing —
        // stay in the current state (hysteresis hold).
    }

    // Handle object whose IR never dropped before 180 degrees
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
}

// -------------------------------------------------------
// MAIN
// -------------------------------------------------------
int main(void) {
    timer_init();
    lcd_init();

    // Allocate and init roomba
    oi_t *roomba = oi_alloc();
    oi_init(roomba);

    cyBOT_init_Scan(0b0111);
    uart_interrupt_init();

    // !! These calibration values are bot-specific !!
    // Run cyBOT_SERVO_cal() if your servo is not aligned.
    right_calibration_value = 232750;
    left_calibration_value  = 1204000;

    command_byte = 'm';

    // Allocate scan struct on heap
    cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));

    // FIX: original code had these as uninitialized pointers
    // (float *distance, int *smallestAngle, char *buffer[100])
    // which would cause a crash. Declared correctly here as
    // stack-allocated values/arrays.
    Object objects[MAX_OBJECTS];
    int    objCount      = 0;
    int    smallestIndex = -1;
    char   buffer[100];

    // Do an initial scan to home the servo
    cyBOT_Scan(0, getScan);

    while (1) {
        if (command_flag == 1) {

            detectObject(getScan, objects, &objCount, &smallestIndex, buffer);

            if (smallestIndex != -1) {
                int targetAngle    = objects[smallestIndex].midAngle;
                float targetDist   = objects[smallestIndex].distance;

                // Convert cm -> mm, stop 10cm (100mm) short
                double driveDistance = (targetDist * 10.0) - 100.0;

                snprintf(buffer, 100, "Turning to %d deg, driving %.1f mm\r\n",
                         targetAngle, driveDistance);
                uart_sendStr(buffer);

                // 90 degrees is straight ahead on the servo.
                // Turn to face the object before driving.
                if (targetAngle < 90) {
                    // Object is to the right
                    turn_Clockwise(roomba, 90 - targetAngle);
                } else if (targetAngle > 90) {
                    // Object is to the left
                    turn_CntrClockwise(roomba, targetAngle - 90);
                }
                // If exactly 90, already facing it — no turn needed

                timer_waitMillis(500);

                // Drive to object, avoiding bumps along the way
                if (driveDistance > 0) {
                    bumpSense(roomba, driveDistance, 150);
                }

                uart_sendStr("Reached target!\r\n");
                lcd_printf("Done!\n%d deg", targetAngle);
            }

            command_flag = 0;
        }
    }
}
