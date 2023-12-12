#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"

void initGPIO(void);

void initGPIO(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Set the clock for GPIOF peripheral
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x0C); // Set as outputs PF2 and PF3
}

int main(void) {
    uint32_t i;
    initGPIO(); // Initialize the peripheral
    while (1) { // Continuous behavior
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0x08); // ON PF1 OFF PF2
        for (i = 0; i < 1000000; i++); // Wait
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0x04); // OFF PF1 ON PF2
        for (i = 0; i < 1000000; i++); // Wait
    }
}
