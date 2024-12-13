// Testprogram two LCD Displays 480x272 and 800x480 RGB
// Display controller Type SSD 1963 => Solomon Systech
// Initialisation + Single Pixel and Rectangle Output
// ARM-Contoller Clock not changed ... to be tested
// Based partially of tests and sources from Ole Roenna 2020
// V0.1-V0.4 K.R. Riemschneider .. 1-4 Oct 2020

// Size of Display define
#define LARGE_DISPLAY

#include <stdint.h>
#include <stdbool.h> // type bool for giop.h
#include "inc/hw_types.h"
#include "inc/tm4c1294ncpdt.h"
#include <stdio.h>   // Debug only
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>     // GPIO_PIN_X
#include <inc/hw_memmap.h>      // GPIO_PORTX_BASE
#include "Display.h"
#include "Adc.h"

void main(void)
{

    int j;
    sysClock = SysCtlClockFreqSet(
    SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480,
                                  120000000); // Set system frequency to 120 MHz

    // Init Port L for Display Control and Port M for Display Data
    init_ports_display();

    // display design
    show_default_display();

    // Start endless loop
    j=0;
    while (1)
    {
    }
}
