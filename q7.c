#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"

uint8_t state=1; //3 possible states: 1(red) 2(blue) 3(green)
uint8_t period=1; //Changes the period of the clock that says when measurements are taken from LDRs (can be 1,5 or 10s)
uint8_t active=1; //active = 1 (LED on) active = 0 (LED off)
uint32_t duty = 0;
uint32_t adcValues[4] = {0}; // ADC conversion result array

void myButtonHandler(void) {
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
    if (state == 1) {
        state = 2;
        period = 5;
    } else if (state == 2) {
        state = 3;
        period = 10;
    } else {
        state = 1;
        period = 1;
    }
    TimerLoadSet(TIMER0_BASE, TIMER_A, period * 16000000);
}

void initGPIO(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_4, GPIO_RISING_EDGE);
    GPIOIntRegister(GPIO_PORTF_BASE, myButtonHandler);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
}

void initPWM(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypePWM(GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPinConfigure(0x00001C05);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_8);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, 2000);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3, 0);
    PWMOutputState(PWM1_BASE, PWM_OUT_3_BIT, true);
}

void initADC(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCIntEnable(ADC0_BASE, 1);
    ADCSequenceEnable(ADC0_BASE, 1);
}

void timer0Handler() {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    active = !active;
    if (active) {
        TimerLoadSet(TIMER0_BASE, TIMER_A, period * 16000000);
        ADCProcessorTrigger(ADC0_BASE, 1);
    }
}

void initTimer0(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, period * 16000000);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE, TIMER_A, timer0Handler);
    TimerEnable(TIMER0_BASE, TIMER_A);
}

int main(void) {
    initGPIO();
    initTimer0();
    initPWM();
    PWMGenEnable(PWM1_BASE, PWM_GEN_1);
    initADC();

    uint32_t adcValues[4] = {0};
    uint32_t duty = 0;

    while(1) {
        if (active == 0) {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
        } else {
            while(ADCBusy(ADC0_BASE));
            ADCSequenceDataGet(ADC0_BASE, 1, adcValues);
            if (state == 1) {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x02); // Red
                duty = adcValues[0];
            } else if (state == 2) {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x04); // Blue
                duty = adcValues[1];
            } else {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x08); // Green
                duty = adcValues[2];
            }

        }
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3, duty);
    }
}
