#include "adc.h"
#include "REF_tm4c123gh6pm.h"

void adc_init(void) {
    SYSCTL_RCGCGPIO_R |= 0x02;  
    SYSCTL_RCGCADC_R |= 0x01;    
    while((SYSCTL_PRGPIO_R & 0x02) == 0) {};
    while((SYSCTL_PRADC_R & 0x01) == 0) {};

    GPIO_PORTB_DIR_R &= ~0x10;  
    GPIO_PORTB_AFSEL_R |= 0x10;  
    GPIO_PORTB_DEN_R &= ~0x10;   
    GPIO_PORTB_AMSEL_R |= 0x10;  

    ADC0_ACTSS_R &= ~0x01;

    ADC0_EMUX_R &= ~0x000F;

    ADC0_SSMUX0_R = 10;

    ADC0_SSCTL0_R = 0x06;

    ADC0_IM_R &= ~0x01;

    ADC0_ACTSS_R |= 0x01;
}

uint16_t adc_read(void) {
    uint16_t result;

    ADC0_PSSI_R = 0x01;

    while((ADC0_RIS_R & 0x01) == 0);

    result = ADC0_SSFIFO0_R & 0x0FFF;

    ADC0_ISC_R = 0x01;

    return result;
}
