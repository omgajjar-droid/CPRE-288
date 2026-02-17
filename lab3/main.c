
#include "open_interface.h"
#include "Timer.h"
#include "lcd.h"
#include <math.h>

void move_forward(oi_t *sensor, int distance) {
    int sum = 0;
    
    oi_update(sensor);
    
    oi_setWheels(200, 200);
    
    while (sum < distance) {
        oi_update(sensor);
        sum += sensor->distance;
        
        if (sensor->bumpLeft || sensor->bumpRight) {
            oi_setWheels(0, 0); 
            lcd_printf("Bump detected!");
            timer_waitMillis(1000);
            return;
        }
        
        if (sensor->cliffLeft || sensor->cliffFrontLeft || 
            sensor->cliffFrontRight || sensor->cliffRight) {
            oi_setWheels(0, 0); 
            lcd_printf("Cliff detected!");
            timer_waitMillis(1000);
            return;
        }
    }
    
    oi_setWheels(0, 0); 
}

void turn(oi_t *sensor, double degrees) {
    double angle_sum = 0;
    
    oi_update(sensor);
    
    if (degrees > 0) {
        oi_setWheels(100, -100);
    } else {
        oi_setWheels(-100, 100);
    }
    
    while (fabs(angle_sum) < fabs(degrees)) {
        oi_update(sensor);
        angle_sum += sensor->angle;
    }
    
    oi_setWheels(0, 0); 
}


void drive_square(oi_t *sensor, int side_len) {
    for (int i = 0; i < 4; i++) {
        lcd_printf("Side %d", i+1);
        move_forward(sensor, side_len);
        timer_waitMillis(500); 
        
        lcd_printf("Turn %d", i+1);
        turn(sensor, 90);  
        timer_waitMillis(500);  
    }
    
    lcd_printf("Square complete!");
}



int main(void) {
    timer_init();
    lcd_init();
    

    oi_t *roomba = oi_alloc();
    oi_init(roomba);
    
    lcd_printf("Roomba Ready!");
    timer_waitMillis(2000);
    
    lcd_clear();
    lcd_printf("Moving forward");
    move_forward(roomba, 500);
    timer_waitMillis(2000);
    
    // lcd_clear();
    // lcd_printf("Driving square");
    // drive_square(roomba, 300);
    // timer_waitMillis(2000);
    
    // oi_free(roomba);
    // lcd_printf("Done!");
    
    
    return 0;
}

