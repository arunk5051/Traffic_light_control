#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


unsigned int i;

void PortBIntHandler()              //isr for portb interrupt
{
    if(GPIOIntStatus(GPIO_PORTB_BASE, true) & GPIO_PIN_5)         //ped request switch - cause of the interrupt
    {
        GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_5);            //clear any interrupts on portb pin 5
        SysCtlDelay(50000000);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);             //traffic green 0
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);    //traffic amber 1
        SysCtlDelay(20000000);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0);             //ped red 0
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);             //traffic amber 0
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_PIN_2);    //traffic red 1
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_PIN_1);    // ped green 1
        SysCtlDelay(100000000);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0);             //ped green 0
    for(i=0; i<= 5; i++)                                          //blink ped red
        {
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_PIN_0);  //ped red 1
            SysCtlDelay(5000000);
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0);           //ped red 0
            SysCtlDelay(5000000);
        }
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);            //traffic red 0
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_PIN_0);   //ped red 1
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4);   //traffic green 1
        SysCtlDelay(5000000);
        GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_5);          //clear any interrupts on portf pin 4 and pin 0
    }
}

void SwIntHandler(void)                     //isr for portf interrupt
{
            unsigned int isr = GPIOIntStatus(GPIO_PORTF_BASE, false);                       //save interrupt status from port f
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);                       //clear any interrupts on portf pin 4 and pin 0

            if((isr & GPIO_PIN_0) && (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2)))           //(sw2 caused the interrupt)Check EW interrupt and SN green status
            {
                SysCtlDelay(50000000);
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);           // EW traffic green 0
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_PIN_6);  // EW traffic amber 1
                SysCtlDelay(20000000);
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 0);           //EW traffic amber 0
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5);  //EW traffic red 1
                SysCtlDelay(500000);
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_PIN_4);  // NS green 1
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);           // NS RED 0
                GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);              //clear any interrupts on portf pin 0
            }

            if(isr & GPIO_PIN_4 && (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5)))          //(sw1 caused the interrupt)Check NS interrupt and EW green status
            {
                SysCtlDelay(100000000);
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);               // NS green 0
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);      // NS AMBER 1
                SysCtlDelay(20000000);
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);               // NS amber 0
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2);      // NS RED 1
                SysCtlDelay(500000);
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0);               // EW RED 0
                GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);      // EW traffic green 1
                GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);                  //clear any interrupts on portf pin 4
            }
}


int main(void)
 {
    SysCtlClockSet(SYSCTL_SYSDIV_2_5| SYSCTL_USE_PLL| SYSCTL_OSC_INT| SYSCTL_XTAL_16MHZ);   //clock setup

   //gpio setup
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enabling port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //enabling port B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //enabling port A
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);          //setting pins 2 to 3 of port A to be an output
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);                     //setting pins 1,2 and 3 of port A to be an output
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_5);  //PED SW


    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;   //
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x1;              //unlocking pin F0
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;               //

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4);    //set gpio port F pin 0 and 4 as input

    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);   //configure portf pin 4 and enable pull up resistor
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);   //configure portf pin 0 and enable pull up resistor

    //interrupt setup

    GPIOIntRegister(GPIO_PORTB_BASE, PortBIntHandler);      //register interrupt on port B as PortBIntHandler
    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_5,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);   //configure portb pin 5 and enable pull up resistor
    GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_5,GPIO_RISING_EDGE);    //trigger interrupts on falling edge
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_5);                 //enable interrupt on portb pin 5

    //---------------

    GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_FALLING_EDGE);     //trigger interrupt on falling edge
    GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_FALLING_EDGE);     //trigger interrupt on falling edge

    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0);                 //enable interrupt on portf pin 0
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);                 //enable interrupt on portf pin 4

   IntEnable(INT_GPIOF);                                           //enable interrupt on portf
 //   IntMasterEnable();                                              //enable master interrupt

    //----------------initial state

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_PIN_0);  //pedestrian red
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0);           //pedestrian green
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);  //pedestrian traffic red
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);           //pedestrian traffic amber
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4);  //pedestrian traffic green


    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2);  //SN traffic red
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);           //SN traffic amber
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);           //SN traffic green
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0);           //EW traffic red
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 0);           //EW traffic amber
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);  //EW traffic green

    //------------------------

    while(1)
    {
    }
}

