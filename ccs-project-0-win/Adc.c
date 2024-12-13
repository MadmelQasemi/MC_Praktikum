/*
 * Adc.c
 *
 *  Created on: 13.12.2024
 *      Author: qasem
 */
#include "Adc.h"

unsigned int ADCValues_PE2[NUM_SAMPLES];
unsigned int ADCValues_PE3[NUM_SAMPLES]; // Array to save the messaurments
unsigned int TauValue_PE1; // Tau-Wert von PE1

SignalProperties signalPE2, signalPE3; // struct for

void initialize_adc()
{
    int w;
    // Port E 2 and 3 for Vsin and Vcos
    SYSCTL_RCGCGPIO_R |= 0x00000010;
    // clock for ADC0
    SYSCTL_RCGCADC_R |= 0x1;
    w++;

    // Magic Code to initialise the ADC-clk
    SYSCTL_PLLFREQ0_R |= SYSCTL_PLLFREQ0_PLLPWR;  // PLL power on
    while (!(SYSCTL_PLLSTAT_R & SYSCTL_PLLSTAT_LOCK))
        ;
    // waite on PLL
    ADC0_CC_R = 0x01; // PIOSC (16 MHz) as ADC-clk source
    SYSCTL_PLLFREQ0_R &= ~SYSCTL_PLLFREQ0_PLLPWR; // PLL power off
    // end of Magic Code

    // configure PE1 for Tau
//    GPIO_PORTE_AHB_AFSEL_R |= 0x02; // activating alternative function for PE1
//    GPIO_PORTE_AHB_AMSEL_R |= 0x02; // Analog mode for PE1
//    GPIO_PORTE_AHB_DEN_R &= ~0x02;  // disable Digital function PE1
//    GPIO_PORTE_AHB_DIR_R &= ~0x02;  // PE1 as input

    // configure PE2 for ADC Vsin
    GPIO_PORTE_AHB_AFSEL_R |= 0x04; // activating alternative function for PE2
    GPIO_PORTE_AHB_AMSEL_R |= 0x04; // Analog mode for PE2
    GPIO_PORTE_AHB_DEN_R &= ~0x04;  // disable Digital function PE2
    GPIO_PORTE_AHB_DIR_R &= ~0x04;  // PE2 as input

    // configure PE3 for ADC Vcos
    GPIO_PORTE_AHB_AFSEL_R |= 0x08; // activating alternative function for PE3
    GPIO_PORTE_AHB_AMSEL_R |= 0x08; // Analog mode for PE3
    GPIO_PORTE_AHB_DEN_R &= ~0x08;  // disable Digital function PE3
    GPIO_PORTE_AHB_DIR_R &= ~0x08;  // PE3 as input

    // ADC-Konfiguration
    ADC0_ACTSS_R &= ~0x0F;          // deactivate all sequencers to configure later
    ADC0_SSMUX0_R = 0x0000000;      // set Sequencer SS0  to AIN1 (PE2)
    ADC0_SSCTL0_R = 0x00000002;     // sequence length = 1
    ADC0_CTL_R &= ~0x01;            // Vdda (3.3V) -> use as Vref
    ADC0_ACTSS_R |= 0x01;           // activate Sequencer SS0

    while (i < NUM_SAMPLES)
    {
        // Messung von PE2 (AIN2)
        ADC0_SSMUX0_R = 0x02;// Kanal AIN2 (PE2)
        ADC0_PSSI_R |= 0x01;// Start des ADC
        while (ADC0_SSFSTAT0_R & 0x00000100)
        ;// Warten, bis FIFO nicht leer ist
        ADCValues_PE2[i] = (uint16_t) ADC0_SSFIFO0_R;// Wert aus FIFO lesen

        delay_us(delay);// Wartezeit

        // Messung von PE3 (AIN3)
        ADC0_SSMUX0_R = 0x03;// Kanal AIN3 (PE3)
        ADC0_PSSI_R |= 0x01;// Start des ADC
        while (ADC0_SSFSTAT0_R & 0x00000100)
        ;// Warten, bis FIFO nicht leer ist
        ADCValues_PE3[i] = (uint16_t) ADC0_SSFIFO0_R;// Wert aus FIFO lesen

        delay_us(delay);// Wartezeit
        i++;
    }
}

// Tau-Signal auslesen (PE1)
//ADC0_SSMUX0_R = 0x01;       // Kanal AIN1 (PE1)
//ADC0_PSSI_R |= 0x01;        // Start des ADC
//while (ADC0_SSFSTAT0_R & 0x00000100)
//;// Warten, bis FIFO nicht leer ist
//TauValue_PE1 = (uint16_t) ADC0_SSFIFO0_R; // Tau-Wert aus FIFO lesen

// Analyse der Signale

analyze_signal(ADCValues_PE2, TauValue_PE1, &signalPE2);
analyze_signal(ADCValues_PE3, TauValue_PE1, &signalPE3);

// Ergebnisse ausgeben
printf("Signal PE2: Amplitude = %.2f mV, Frequenz = %.2f Hz\n",
        signalPE2.amplitude, signalPE2.frequency);
printf("Signal PE3: Amplitude = %.2f mV, Frequenz = %.2f Hz\n",
        signalPE3.amplitude, signalPE3.frequency);
// Ausgabe aller Werte nach Abschluss der Messungen
for (i = 0; i < NUM_SAMPLES; i++)
{
    printf("PE2 Messwert %4d: 0x%03x = %04d [mV]\n", i, ADCValues_PE2[i],
            (int) ((ADCValues_PE2[i] * V_LSB - 1645)) * 3, 5);
    printf("PE3 Messwert %4d: 0x%03x = %04d [mV]\n", i, ADCValues_PE3[i],
            (int) ((ADCValues_PE3[i] * V_LSB - 1645)) * 3, 5);
}
}

// Funktion zur Erzeugung einer Verzögerung in Mikrosekunden
void delay_us(uint32_t us)
{
uint32_t delayTicks = us * (120 / 3); // 120 MHz / 3 (1 Zyklus für jeden Instruktionsschritt)
while (delayTicks--)
    ;

}

void analyze_signal(const unsigned int *data, unsigned int tauValue,
                    SignalProperties *result)
{
int maxVal = 0, minVal = ADC_VREF, voltage;
int i = 0;
for (i = 0; i < NUM_SAMPLES; i++)
{
    voltage = (data[i] * V_LSB - 1645) * 3, 5;
    if (voltage > maxVal)
        maxVal = voltage;
    if (voltage < minVal)
        minVal = voltage;
}
result->amplitude = (maxVal - minVal) / 2.0;

// Frequenz berechnen aus Tau
double tauVoltage = tauValue * V_LSB; // Tau in Spannung umrechnen
double tau = tauVoltage / ADC_VREF;  // Normalisierte Zeitkonstante
result->frequency = 1.0 / (2 * M_PI * tau);
}

