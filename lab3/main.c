#include "open_interface.h"
#include "cyBot_uart.h"
#include "cyBot_Scan.h"
#include <stdio.h>
#include "movement.h"
#include "Timer.h"
#include "lcd.h"
#include <stdlib.h>

void cyBot_sendString(char *input) {
 while(*input != '\0') {
        cyBot_sendByte(*input);
        input++;
    }
}

int main(void) {
    timer_init();
    lcd_init();
    cyBot_uart_init();
    cyBOT_init_Scan(0b0011);
    right_calibration_value = 85750;
     left_calibration_value = 1513750;
     char buffer[100];

    cyBOT_Scan_t *getScan = calloc(1, sizeof(cyBOT_Scan_t));
    int i =0;
    while(1) {
        if(cyBot_getByte() == 'm') {
        cyBOT_Scan(i, getScan);
        i += 5;

            sprintf(buffer, "%d\r\t\n", getScan);
            cyBot_sendString(buffer);

        if(i == 180) {
            break;
        }
    }
    }
//    char buffer[100];
//    while(1){
//        char test = (char) cyBot_getByte();
//
//    sprintf(buffer, "Got an %c", test);
//    cyBot_sendString(buffer);
//    if(cyBot_getByte() == 'p'){
//        break;
//    }
//}
    return 0;
}

