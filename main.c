/****************************************************************************************
 *                      main.c for ECE 266 Lab 7, ULTRA SONIC SENSOR                    *
 *                                                                                      *
 *                      Created on: Nov. 7 2017                                         *
 *                      Group 18                                                        *
 *                                                                                      *
 ***************************************************************************************/


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/systick.h>
#include <driverlib/adc.h>
#include <driverlib/timer.h>
#include "launchpad.h"
#include "seg7.h"
#include "seg7digit.h"
#include "ranger.h"

/*******************************************************
 * emun to switch between different modes of operation *
 ******************************************************/
enum { Centimeter, Inches } displayMode = Centimeter;

/**************************************************************
 * This function I created to use Sw1 and Sw2 to change from  *
 *  Raw mode to Percent mode on the display                   *
 **************************************************************/
void buttonSwitch()
{
    if (pbRead() == 1)
    {
        displayMode = Centimeter;
    }
    if (pbRead() == 2)
    {
        displayMode = Inches;
    }

}

/*************************************************
 * displayVal function was created to change the *
 * value if in Raw mode or in Percent mode       *
 ************************************************/
uint16_t displayVal()
{

    buttonSwitch();
    if (displayMode == Centimeter)
    {
        return (rangerCALC()); // distance in centimenters
    }
    else
    {
        return (rangerCALC() * 0.393701); // distance in inches
    }
}
//declare seg7Display to access the structure
seg7Display_t seg7Display;

/**************************************************************************
 * readValue function is a schdCallback function.  With oneStep which is a *
 * 16 bit unsigned integer equal to the diplayVal() which will            *
 * send the Raw data or the Percent data seen on the display, I set       *
 * oneStep equal to each member in the structure. A %10 and /10 to get the*
 * 10 digit decimal for the corresponding block on the display.            *
 *************************************************************************/
void readValue(uint32_t time)
{

    uint32_t delay = 100;
    uint32_t oneStep;
    uint8_t D2;
    uint8_t D3;
    oneStep = displayVal();



    uprintf("%d\n\r", rangerCALC());

    seg7Display.d1 = oneStep % 10;
    oneStep = oneStep / 10;

    D2 = oneStep % 10;
    oneStep = oneStep / 10;
    seg7Display.d2 = (displayVal() >= 10) ? D2 : 32;

    D3 = oneStep % 10;
    oneStep = oneStep / 10;
    seg7Display.d3 = (displayVal() >= 100) ? D3 : 32;

    seg7Display.d4 = oneStep % 10;
    oneStep = oneStep / 10;

    seg7DigitUpdate(&seg7Display);
    schdCallback(readValue, time + delay);

}

int main(void)
{
    lpInit();
    rangerInit();
    seg7Init();


    uprintf("%s\n\r", "Lab 7: UltraSonic Sensor" );
    schdCallback(readValue, 150);

    // Run the event scheduler to process callback events
    while (true)
    {
        //rangerCALC();
        schdExecute();
    }
}
