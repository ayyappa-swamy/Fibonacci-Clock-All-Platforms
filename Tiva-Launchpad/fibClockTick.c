//*****************************************************************************
//
// interrupt.c - Example demonstrating how to configure the systick interrupt.
//
// Copyright (c) 2010-2016 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This is part of revision 2.1.3.156 of the Tiva Firmware Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "buttons.h"
//*****************************************************************************
//
//! \addtogroup systick_examples_list
//! <h1>Systick Interrupt (systick_int)</h1>
//!
//! This example shows how to configure the SysTick and the SysTick interrupt.
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - NONE
//!
//! The following UART signals are configured only for displaying console
//! messages for this example.  These are not required for operation of
//! Systick.
//! - UART0 peripheral
//! - GPIO Port A peripheral (for UART0 pins)
//! - UART0RX - PA0
//! - UART0TX - PA1
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - SysTickIntHandler
//
//*****************************************************************************

#define NONE 0
#define HOURS 1
#define MINUTES 2
#define BOTH 3

//*****************************************************************************
//
// Counter to count the number of minutes and hours that have been counted.
//
//*****************************************************************************
volatile uint32_t g_minuteCounter = 0;
volatile uint32_t g_hourCounter = 0;

struct numArray
{
	int number;
	int flag;
	uint32_t PORT_ADDR;
	uint32_t RED_PIN_ADDR;
	uint32_t GREEN_PIN_ADDR;
	uint32_t BLUE_PIN_ADDR;
}fibArray[5] = {
					{5,	NONE, GPIO_PORTE_BASE, 0x00000001, 0x00000002, 0x00000004},
					{3, NONE, GPIO_PORTE_BASE, 0x00000008, 0x00000010, 0x00000020},
					{2, NONE, GPIO_PORTC_BASE, 0x00000010, 0x00000020, 0x00000040},
					{1, NONE, GPIO_PORTL_BASE, 0x00000001, 0x00000002, 0x00000004},
					{1, NONE, GPIO_PORTL_BASE, 0x00000008, 0x00000010, 0x00000020}
};

struct numArray g_fibList[5] = {
		{5,	NONE, GPIO_PORTE_BASE, 0x00000001, 0x00000002, 0x00000004},
		{3, NONE, GPIO_PORTE_BASE, 0x00000008, 0x00000010, 0x00000020},
		{2, NONE, GPIO_PORTC_BASE, 0x00000010, 0x00000020, 0x00000040},
		{1, NONE, GPIO_PORTL_BASE, 0x00000001, 0x00000002, 0x00000004},
		{1, NONE, GPIO_PORTL_BASE, 0x00000008, 0x00000010, 0x00000020}
};

typedef struct numArray * NUMARRAY;

//NUMARRAY g_fibList;

/**
  * This function converts the given 'number' (0-11) into its fibonacci series representation
  * and updates the array 'auxillaryList'
  *
  * 'unit' - This parameter is used to update the flag in the structure
  *        - unit == 1 => for hours
  *        - unit == 2 => for minutes
 **/
void convertNumberToFibList(int number, NUMARRAY auxillaryList, int unit)
{
	int index = 0;
	int sum = 0;

	while(index < 5){
		sum += fibArray[index].number;

		if(sum > number){
			sum -= fibArray[index].number;
		}
		else if(sum == number){
			auxillaryList[index].flag += unit;
			break;
		}
		else{
			auxillaryList[index].flag += unit;
		}
		index++;
	}
	return;
}

//*****************************************************************************
//
// This initializes the NUMARRAY fibList to the global array fibArray
//
//*****************************************************************************
void
initFibList(NUMARRAY fibList)
{
	uint8_t index = 0;
	for (index = 0; index < 5; index++){
		fibList[index].number = fibArray[index].number;
		fibList[index].PORT_ADDR  = fibArray[index].PORT_ADDR;
		fibList[index].flag = fibArray[index].flag;
		fibList[index].RED_PIN_ADDR = fibArray[index].RED_PIN_ADDR;
		fibList[index].BLUE_PIN_ADDR = fibArray[index].BLUE_PIN_ADDR;
		fibList[index].GREEN_PIN_ADDR = fibArray[index].GREEN_PIN_ADDR;
	}
}

//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
void
InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

void LEDsInit(void)
{
    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x00);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))
    {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_6, 0x00);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL))
    {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_5);
    GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, 0x00);
    GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_1, 0x00);
    GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2, 0x00);
    GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_3, 0x00);
    GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_4, 0x00);
    GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_5, 0x00);
}

//*****************************************************************************
//
// The function to update the LED pins based on the values of g_fibList
//
//*****************************************************************************
void updateLEDs(void)
{
	int index = 0;
	UARTprintf("\n ---- updating leds -----");

	for(index = 0; index < 5; index++){

		UARTprintf("\n num:%d, flag:%d ",g_fibList[index].number,g_fibList[index].flag);
		switch(g_fibList[index].flag){

		case NONE :
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].GREEN_PIN_ADDR,0x00);
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].BLUE_PIN_ADDR,0x00);
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].RED_PIN_ADDR, 0x00);
					break;
		case HOURS :
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].GREEN_PIN_ADDR,g_fibList[index].GREEN_PIN_ADDR);
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].BLUE_PIN_ADDR,0x00);
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].RED_PIN_ADDR, 0x00);
					break;
		case MINUTES :
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].GREEN_PIN_ADDR,0x00);
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].BLUE_PIN_ADDR,g_fibList[index].BLUE_PIN_ADDR);
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].RED_PIN_ADDR, 0x00);
					break;
		case BOTH :
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].GREEN_PIN_ADDR,0x00);
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].BLUE_PIN_ADDR,0x00);
					GPIOPinWrite(g_fibList[index].PORT_ADDR,g_fibList[index].RED_PIN_ADDR, g_fibList[index].RED_PIN_ADDR);
					break;
		}

	}

	UARTprintf("\n updated LEDs");
}

//*****************************************************************************
//
// The interrupt handler for the for Systick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Update the Systick minute and hour counters.
    //
	/*if(g_minuteCounter + 5 >= 60){
		if(g_hourCounter + 1 > 12){
			g_hourCounter = 0;
		}
		else{
			g_hourCounter += 1;
		}
		g_minuteCounter = 0;
	}
	else{
		g_minuteCounter += 5;
	}*/
	UARTprintf("\n\n ---- in ticker -----");
	g_hourCounter = 11;
	g_minuteCounter = 30;

	//Initialize the flags of g_fibList to NONE
	int index = 0;
	for (index = 0; index < 5; index++){
		g_fibList[index].flag = NONE;
	}

	UARTprintf("\n ---- converting hours to fib list -----");

	//convert the current hours value and generate the fibonacci series representation
	convertNumberToFibList(g_hourCounter, g_fibList, 1);

	UARTprintf("\n ---- converting minutes to fib list -----");
	//convert the current minutes value and generate the fibonacci series representation
	convertNumberToFibList(g_minuteCounter/5, g_fibList, 2);

	//update the LEDs with current minute and hour values
    updateLEDs();
}

//*****************************************************************************
//
// Configure the SysTick and SysTick interrupt with a period of 1 second.
//
//*****************************************************************************
int
main(void)
{
#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
    uint32_t ui32SysClock;
#endif

    uint32_t ui32PrevCount = 0;

    //
    // Set the clocking to run directly from the external crystal/oscillator.
    //
#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_OSC), 25000000);
#else
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
#endif

    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for Systick operation.
    //
    InitConsole();

    //
    // Display the setup on the console.
    //
    UARTprintf("SysTick Firing Interrupt ->");
    UARTprintf("\n   Rate = 1sec\n\n");

    //
    // Initialize the interrupt counter.
    //
    g_minuteCounter = 0;
    g_hourCounter = 0;
    LEDsInit();


    //Initialize global fibonacci list to store the structure
    //g_fibList = (NUMARRAY)calloc(5,sizeof(NUMARRAY));

    //initFibList(g_fibList);

    //
    // Set up the period for the SysTick timer.  The SysTick timer period will
    // be equal to the system clock, resulting in a period of 1 second.
    //
#if defined(TARGET_IS_TM4C129_RA0) ||                                         \
    defined(TARGET_IS_TM4C129_RA1) ||                                         \
    defined(TARGET_IS_TM4C129_RA2)
    SysTickPeriodSet(ui32SysClock);
#else
    SysTickPeriodSet(SysCtlClockGet()*100); //System Clock is set to 1*60*5 seconds == 5 minutes
#endif

    //
    // Enable interrupts to the processor.
    //
    IntMasterEnable();

    //
    // Enable the SysTick Interrupt.
    //
    SysTickIntEnable();

    //
    // Enable SysTick.
    //
    SysTickEnable();

    //
    // Loop forever while the SysTick runs.
    //
    while(1)
    {

    }
}
