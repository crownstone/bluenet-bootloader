/**
 * Author: Anne van Rossum
 * Copyright: Distributed Organisms B.V. (DoBots)
 * Date: 10 Oct., 2014
 * License: LGPLv3+, Apache, and/or MIT, your choice
 */

#include <serial.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf51.h"
//#include "nRF51822.h"
#include "dobots_boards.h"

#define NRF_UART_9600_BAUD  0x00275000UL
#define NRF_UART_38400_BAUD 0x009D5000UL

/**
 * Configure the UART. Currently we set it on 38400 baud.
 */
void config_uart() {
	// Enable UART
	NRF_UART0->ENABLE = 0x04; 

	// Configure UART pins
	NRF_UART0->PSELRXD = PIN_RX;  
	NRF_UART0->PSELTXD = PIN_TX; 

	//NRF_UART0->CONFIG = NRF_UART0->CONFIG_HWFC_ENABLED; // do not enable hardware flow control.
	NRF_UART0->BAUDRATE = NRF_UART_38400_BAUD;
	NRF_UART0->TASKS_STARTTX = 1;
	NRF_UART0->TASKS_STARTRX = 1;
	NRF_UART0->EVENTS_RXDRDY = 0;
	NRF_UART0->EVENTS_TXDRDY = 0;
}

void write_token(const char token) {
	NRF_UART0->TXD = (uint8_t)token;
	while(NRF_UART0->EVENTS_TXDRDY != 1) {}
	NRF_UART0->EVENTS_TXDRDY = 0;
}

void write_string(const char *str, int len) {
	for(int i = 0; i < len; ++i) {
		NRF_UART0->TXD = (uint8_t)str[i];
		while(NRF_UART0->EVENTS_TXDRDY != 1) {}
		NRF_UART0->EVENTS_TXDRDY = 0;
	}
}

