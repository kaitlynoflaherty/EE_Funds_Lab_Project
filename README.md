# Electronics Engineering Fundamentals Lab Project: Light detector and control

## Introduction
The laboratory project for this course was to implement an electronic system capable of 
measuring and adjusting light level. To implement the system, we worked with the 
LaunchPad TM4C123 from Texas Instruments used during the course. We used 3 light 
dependent resistors (LDR) to measure the light level, a white LED to adjust light 
level, a push button for modifying the system operation, and an RGB LED to indicate 
the different system states.

## Learning Objectives of this lab
- Understanding 
  - GPIOs
  - interrupts
  - timers
  - PWM
  - ADC
  - LDR behavior
- Connecting external sensors and actuator to the microcontroller
- Understanding the relationship between the hardware and the software
- Practicing with the Code Composer Studio (CCS) and C language
## Steps Included in this Repo
### Q1: 
Alternate the RGB LED color between green and blue every 1 second (approximately).
### Q2: 
Change the LED color between blue and green each time you pushthe button.
### Q3: 
Alternate the RGB LED colors every 1 second using timers and interruptions.
### Q4: 
Alternate the RGB LED colors using the push button. But now, for every color,
the blinking period must be the one related to the sensor assigned. You must use 
timers and interruptions.
### Q5: 
Adjust brightness level for the white LED using PWM signals. Select 3 levels of 
brightness (off, medium and high) and calculate the PWM parameters needed to perform 
this brightness control.
### Question 6: 
Using previous work as a project base, schedule a sensor measurement every 5 seconds 
using ADC, timers and interruptions. This measurement must include the three sensors.
### Question 7: 
Implement the final program that select the sensor to measure using the push 
button, launches a sensor measurement using the sensing period assigned and control the 
white LED brightness according to the measured light level. Depict the sensor selected 
with the RGB LED that must blink every time a measurement procedure is launched.
