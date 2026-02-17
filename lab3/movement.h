

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "open_interface.h"

void move_forward(oi_t *sensor, int distance);
void move_backward(oi_t *sensor, int distance);
void turn(oi_t *sensor, double degrees);

void drive_square(oi_t *sensor, int side_len);

void obstacle_avoidance(oi_t *sensor);
void follow_wall(oi_t *sensor, int duration_ms);

void stop_roomba(oi_t *sensor);
int calculate_turn_time(double degrees);

#endif


