
#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "open_interface.h"

double move_backward(oi_t*sensor_data, double distance_mm, int speed);
void turn_Clockwise(oi_t*sensor_data, double degrees);
void turn_CntrClockwise(oi_t*sensor_data, double degrees);
double move_forward(oi_t*sensor_data, double distance_mm, int speed);
void bumpSense(oi_t*sensor_data, double distance_mm, int speed);
#endif
