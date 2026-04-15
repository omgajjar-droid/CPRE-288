/*
 * servo.h
 *
 *  Created on: Apr 8, 2026
 *      Author: bmkunka
 */

#ifndef SERVO_H_
#define SERVO_H_

void servo_init(void);
void servo_move(uint16_t degrees);
void servo_calibration(void);



#endif /* SERVO_H_ */
