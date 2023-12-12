
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

uint8_t button = 0;
uint8_t blue = 1;


void buttonHandler(){
 GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
 button = 1;
 }


// to release the signal: 1
// to press the signal: 0

int main(void)
  {
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3);
GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA,
GPIO_PIN_TYPE_STD_WPU);

//Now, we need to configure the interrupts
GPIOIntDisable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
GPIOIntRegister(GPIO_PORTF_BASE, buttonHandler);
GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);


while(1){
 if (button == 1){
        switch(blue){
            case 0:
GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3);
blue = 1;
break;
            case  1:
GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_2);
blue =0;
break;
}
 button = 0;
 }
}
}
