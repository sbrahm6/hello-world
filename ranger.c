/*
 * ranger.c
 *
 *  Created on: Nov 8, 2017
 *      Author: cary
 */




#include  <stdint.h>
#include  <stdbool.h>
#include  <stdio.h>
#include  <inc/hw_memmap.h>

#include  <driverlib/gpio.h>
#include  <driverlib/pin_map.h>
#include  <driverlib/sysctl.h>
#include  <driverlib/systick.h>
#include  <driverlib/adc.h>
#include  <driverlib/timer.h>
#include  "ranger.h"
#include  "launchpad.h"
#include  "seg7.h"
#include "seg7digit.h"




#define LOW             0
#define HIGH            1
//#define TIMER_PERIPH    SYSCTL_PERIPH_WTIMER0
//#define PORT_PERIPH     SYSCTL_PERIPH_GPIOC



void rangerInit() {

    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);


    TimerConfigure(WTIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP));
    TimerControlEvent(WTIMER0_BASE,TIMER_A,TIMER_EVENT_BOTH_EDGES);

    TimerEnable(WTIMER0_BASE, TIMER_A);
    TimerIntEnable(WTIMER0_BASE, TIMER_A);

}
//Trigger and record the device data
uint32_t rangerCALC() {



    uint32_t rangerCALC  = 0;
    uint32_t risingEdge  = 0;
    uint32_t fallingEdge = 0;



    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, LOW);
    waitUs(2);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);
    waitUs(5);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, LOW);

    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinConfigure(GPIO_PC4_WT0CCP0);

    TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);
    while (!TimerIntStatus(WTIMER0_BASE, false))


    risingEdge = TimerValueGet(WTIMER0_BASE, TIMER_A);

    TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);
    while (!TimerIntStatus(WTIMER0_BASE, false))


    fallingEdge = TimerValueGet(WTIMER0_BASE, TIMER_A);

    TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);


   /* while (risingEdge >= fallingEdge)
    {
    }
    distance = (distance) / 2;
*/

    rangerCALC = (fallingEdge - risingEdge);

    rangerCALC = ((rangerCALC * 340) / 2) * 100;        // meters to centimeters


return rangerCALC;
}
