#include "open_interface.h"
#include "lcd.h"
#include "timer.h"


double move_forward(oi_t*sensor_data, double distance_mm, int speed)
{

    oi_setWheels(speed,speed);

    double sum = 0;
    while(sum < distance_mm){
        oi_update(sensor_data);

        sum += sensor_data ->distance;
    }

    oi_setWheels(0,0);


    return sum;



}
double move_backward(oi_t*sensor_data, double distance_mm, int speed)
{

    oi_setWheels(-speed,-speed);


    double sum = distance_mm;
    while(sum > 0){

        oi_update(sensor_data);
        sum += sensor_data -> distance;
    }

    oi_setWheels(0,0);

    return sum;



}

void turn_Clockwise(oi_t*sensor_data, double degrees)
{
    degrees = degrees *-1;


    int sum = 0;

    oi_setWheels(-190,190);



    while(sum > degrees){
        oi_update(sensor_data);

        sum += sensor_data ->angle;
    }


    oi_setWheels(0,0);



}
void turn_CntrClockwise(oi_t*sensor_data, double degrees)
{
     int sum = 0;

    oi_setWheels(190,-190);


    while(sum < degrees){
        oi_update(sensor_data);
        sum += sensor_data -> angle;
    }

    oi_setWheels(0,0);
}

void bumpSense(oi_t*sensor_data, double distance_mm, int speed){
    double moved = 0;
    int right_bump = 0;
    int left_bump = 0;

    oi_setWheels(speed,speed);

    while(moved < distance_mm){
        oi_update(sensor_data);
        moved += sensor_data ->distance;
        right_bump = sensor_data ->bumpRight;
        left_bump = sensor_data ->bumpLeft;

        if(left_bump != 0){
            oi_setWheels(0,0);
            move_backward(sensor_data,150,100);
            moved -= 150;

            turn_Clockwise(sensor_data,70);
            move_forward(sensor_data,250,150);

            turn_CntrClockwise(sensor_data,70);
            oi_setWheels(speed,speed);
        }
        if(right_bump != 0){
                   oi_setWheels(0,0);
                   move_backward(sensor_data,150,100);
                   moved -= 150;

                   turn_CntrClockwise(sensor_data,70);
                   move_forward(sensor_data,250,150);

                   turn_Clockwise(sensor_data,70);
                   oi_setWheels(speed,speed);
               }
    }
}
