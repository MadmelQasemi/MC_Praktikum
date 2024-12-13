/*
 * Adc.h
 *
 *  Created on: 13.12.2024
 *      Author: qasem
 */

#ifndef ADC_H_
#define ADC_H_
/*
 * Adc.h
 *
 *  Created on: 13.12.2024
 *      Author: qasem
 */

#ifndef ADC_H_
#define ADC_H_
#include <stdint.h>
#include <stdbool.h> // type bool for giop.h
#include "inc/hw_types.h"
#include "inc/tm4c1294ncpdt.h"
#include <stdio.h>   // Debug only
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>     // GPIO_PIN_X
#include <inc/hw_memmap.h>      // GPIO_PORTX_BASE
#include "Display.h"
#include <math.h>
#define ADC_VREF 3300.0 // Spannung an V_REF+ in mV
#define V_LSB (ADC_VREF / 4096) // Spannung pro LSB in mV

#define NUM_SAMPLES 800 // Anzahl der Messwerte
#define delay 2
#define SAMPLE_RATE 1/(delay/1000000)

typedef struct {
    double amplitude;
    double frequency;
} SignalProperties;

void initialize_adc();
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void analyze_signal(const unsigned int *data, unsigned int tauValue, SignalProperties *result);


#endif /* ADC_H_ */


#endif /* ADC_H_ */
