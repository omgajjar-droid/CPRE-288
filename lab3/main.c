#include "open_interface.h"
#include "cyBot_uart.h"
#include "cyBot_Scan.h"
#include <stdio.h>
#include "movement.h"
#include "Timer.h"
#include "lcd.h"


void cyBOT_sendString(char *input) {
 while(*input != \0) {
        cyBot_sendByte(*input);
        input++;
    }
}

int main(void) {

    
    return 0;
}

