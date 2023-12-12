//Import used libraries
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include <stdio.h>

//HAL functions used
void GPIOPinTypeGPIOOutput (uint32_t ui32Port, uint8_t ui8Pins);
void GPIOPinWrite (uint32_t ui32Port, uint8_t ui8Pins, uint8_t ui8Val);
void GPIOPinTypeGPIOInput (uint32_t ui32Port, uint8_t ui8Pins);
void GPIOPadConfigSet (uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32Strength, uint32_t ui32PinType);
void GPIOIntDisable (uint32_t ui32Port, uint32_t ui32IntFlags);
void GPIOIntClear (uint32_t ui32Port, uint32_t ui32IntFlags);
void GPIOIntRegister (uint32_t ui32Port, void (pfnIntHandler)(void));
void GPIOIntTypeSet (uint32_t ui32Port, uint8_t ui8Pins, uint32_t ui32IntType);
void GPIOIntEnable (uint32_t ui32Port, uint32_t ui32IntFlags);
void SysCtlPeripheralEnable (uint32_t ui32Peripheral);

//Global variables used
uint8_t state=2; //3 possible states: 1(red) 2(blue) 3(green)
uint8_t period=1; //Changes the period of the clock that says when measurements are taken from LDRs (can be 1,5 or 10s)
uint8_t active=1; //active = 1 (LED on) active = 0 (LED off)
uint32_t duty = 0;
uint32_t adcValues[4] = {0}; // ADC conversion result array
uint8_t flag = 0;


//Function that changes state of the system when the button is pressed
void myButtonHandler(void) {
    GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_4); //Clear the pending flag to indicate I am attending the interrupt
    //Change state to the next one (together with period)
    if (state==1) {
                    state=2;
                    period = 5;
                    duty = 1500;

                  } else if (state==2){
                      state = 3;
                      period = 10;
                      duty = 500;

                  }else{
                      state=1;
                      period = 1;
                      duty = 0;

                  }

    TimerLoadSet(TIMER0_BASE,TIMER_A,period*16000000); //Update timer

}


//Initialize GPIO
void initGPIO (void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Set the clock for GPIOF peripheral port F
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,0x0E); //Set as outputs PF1, PF2 and PF3
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4); //Set as inputs PF4
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //Pull down configuration
    GPIOIntDisable(GPIO_PORTF_BASE,GPIO_INT_PIN_4);//First we disable we are configuring in order not to be interrupted
    GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_4);//Clear possible pending flags or previous interrupts that would not have been attended
    GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_INT_PIN_4,GPIO_RISING_EDGE); //We set falling or rising the interrupt
    GPIOIntRegister(GPIO_PORTF_BASE,myButtonHandler); //Function of the button press
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_4);//Enable interrupt
}

// Initialize PWWM
void initPWM(void){

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // enable PWM module 1, Gen 1, module 3
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // enable also the GPIO for PA7
    GPIOPinTypePWM(GPIO_PORTA_BASE, GPIO_PIN_7); // link the use of PA7 to PWM
    GPIOPinConfigure(0x00001C05);// port and module
    SysCtlPWMClockSet(SYSCTL_PWMDIV_8); // clock divider or prescale
    PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet( PWM1_BASE, PWM_GEN_1, 2000); // period
    PWMPulseWidthSet( PWM1_BASE, PWM_GEN_1, 0);
    PWMOutputState(PWM1_BASE, PWM_OUT_3_BIT, true); // enable the output

}


// Initialize ADC
void initADC(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); // enable peripherals and ADC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // enable GPIO that are connected to the ADC
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // link PE1, PE2, PE3 to ADC functionality
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0); // use ss1
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH2); // link PE1 -> AIN2
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH1); // link PE2 -> AIN1
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH0 | ADC_CTL_END); // link PE3 -> AIN0
    ADCSequenceEnable(ADC0_BASE, 1); // enable ss1 and ADC
}

//First timer function (controls when the readings are taken)
void timer0Handler(){
    TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT); //Clears timer interrupt sources (we are attending the interrupt)
    // Toggle the active state for blinking effect
    active = !active;
    // If active, reload the timer for the next timeout
    if (active) {
        TimerLoadSet(TIMER0_BASE, TIMER_A, period * 16000000); // Update timer
        if (state ==2){
            flag = 1;
        }
    }

}

void initTimer0(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); //Enable peripheral
    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM); //Sets source from systems' one
    TimerConfigure(TIMER0_BASE,TIMER_CFG_A_PERIODIC); //Periodic clock
    TimerLoadSet(TIMER0_BASE,TIMER_A,period*16000000); //Set period
    TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT); //Enables interrupt
    TimerIntRegister(TIMER0_BASE,TIMER_A,timer0Handler); //Calls previous function
    TimerEnable(TIMER0_BASE,TIMER_A); //Enables timer
}


int main(void) {
    initGPIO(); //Initialize GPIO
    initTimer0(); //Initialize timer0
    initPWM(); // Initialize PWM
    PWMGenEnable(PWM1_BASE,PWM_GEN_1);
    initADC(); // Initialize ADC


    //Continuous behavior
    while(1) {
        // If active=1 RGB LED is on, if not, it is off
        if (active == 0) {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x01); // LED OFF
        }
        else {
            // Color of the RGB LED changes with the state
            if(state==1) {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x02); //ON PF1 (RED)
            }
            else if (state==2) {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x04); //ON PF2 (BLUE)
            }
            else {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x08); //ON PF3(GREEN)
            }

        }

        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3, duty); // set duty cycle

        if (flag){
            ADCProcessorTrigger(ADC0_BASE,1);
              //We have to wait TILL THE CAPTURES FINISH
            while(ADCBusy(ADC0_BASE));
              //When it finishes, we read and store the values at ADC vector
            ADCSequenceDataGet(ADC0_BASE,1,adcValues);
            flag = 0;
        }


    }
}
