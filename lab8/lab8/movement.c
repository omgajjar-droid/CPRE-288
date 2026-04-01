#include "open_interface.h"
#include "lcd.h"
#include "timer.h"



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
    return;
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
    return;
}

double move_forward(oi_t*sensor_data, double distance_mm, int speed)
{

    oi_setWheels(speed,speed);
//    char status;
    double sum = 0;
    while(sum < distance_mm){
        oi_update(sensor_data);

                if (sensor_data-> bumpLeft) {
                    oi_setWheels(0, 0);
                    lcd_printf("BumpLeft!");
                    timer_waitMillis(1000);
                    move_backward(sensor_data,150,100);
                    turn_Clockwise(sensor_data,70);
//                    return status = 'l';
                }
                else{
                    sum += sensor_data ->distance;
                }

                if (sensor_data-> bumpRight) {
                    oi_setWheels(0, 0);
                    lcd_printf("BumpRight!");
                    timer_waitMillis(1000);
                    move_backward(sensor_data,150,100);
                    turn_CntrClockwise(sensor_data,70);
//                    return status = 'r';
                }
                else{
                    sum += sensor_data ->distance;
                }
        sum += sensor_data ->distance;
    }

    oi_setWheels(0,0);


    return sum;



}

