/**
 * Author: Anne van Rossum
 * Copyright: Distributed Organisms B.V. (DoBots)
 * Date: 4 Nov., 2014
 * License: LGPLv3+, Apache, and/or MIT, your choice
 */

#ifndef CS_BOARDS_T
#define CS_BOARDS_T

// Current accepted BOARD types

#define PCA10001             0
#define NRF6310              1
#define NRF51822_DONGLE      2
#define NRF51822_EVKIT       3
#define RFDUINO              4
#define CROWNSTONE           5
#define NRF51422             6
#define VIRTUALMEMO          7
#define CROWNSTONE2          8
#define CROWNSTONE_SENSOR    9
#define PCA10000             10

// For the bootloader just hardcode for now
//#define BOARD                NRF6310

#ifndef BOARD
#error "Add BOARD=... to Makefile"
#endif

#if(BOARD==RFDUINO)

#define PIN_RED              2                   // this is gpio 2 (bottom pin)
#define PIN_GREEN            3                   // this is gpio 3 (second pin)
#define PIN_BLUE             4                   // this is gpio 4 (third pin)

#endif


#if(BOARD==NRF6310)

#define PIN_LED0             8                   // this is p1.0
#define PIN_LED1             9                   // this is p1.1
#define PIN_LED2            10                   // this is p1.2
#define PIN_LED3            11                   // this is p1.3
#define PIN_LED4            12                   // this is p1.4
#define PIN_LED5            13                   // this is p1.5
#define PIN_LED6            14                   // this is p1.6
#define PIN_LED7            15                   // this is p1.7
#define PIN_ADC              2                   // ain2 is p0.1
#define PIN_RX               16
#define PIN_TX               17

#define PIN_LPCOMP           3                   // ain3 is p0.2 or gpio 2

#endif


#if(BOARD==CROWNSTONE)

#define PIN_LED              3                   // this is p0.03 or gpio 3
#define PIN_ADC              5                   // ain5 is p0.04 or gpio 4
#define PIN_LPCOMP           7                   // ain6 is p0.05 or gpio 5 (changed in from 6 which conflict with uart)
#define PIN_RX               6                   // this is p0.06 or gpio 6
#define PIN_TX               1                   // this is p0.01 or gpio 1

#endif


#if(BOARD==PCA10001)

#define PIN_LED             18                   // this is p0.18 or gpio 18
#define PIN_ADC              2                   // ain2 is p0.01 or gpio 1
#define PIN_LPCOMP           3                   // ain3 is p0.02 or gpio 2
#define PIN_RX              11                   // this is p0.11 or gpio 11
#define PIN_TX               9                   // this is p0.09 or gpio 9
#define PIN_LED_CON         19                   // shows connection state on the evaluation board

#endif

#if(BOARD==NRF51422)

#define PIN_LED             18
#define PIN_ADC              2                   // ain 2 is p0.01 or gpio 1
#define PIN_RX               1
#define PIN_TX               2

#endif

// Sanity check to see if all required pins are defined

#ifndef PIN_ADC
#error "For AD conversion PIN_ADC must be defined"
#endif

#ifndef PIN_LPCOMP
#error "For LP comparison PIN_LPCOMP must be defined"
#endif

#ifndef PIN_RX
#error "For UART, PIN_RX must be defined"
#endif

#ifndef PIN_TX
#error "For UART, PIN_TX must be defined"
#endif


#endif // CS_BOARDS_T
