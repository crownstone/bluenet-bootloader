/**
 * Author: Anne van Rossum
 * Copyright: Distributed Organisms B.V. (DoBots)
 * Date: 10 Oct., 2014
 * License: LGPLv3+, Apache, and/or MIT, your choice
 */

#include <serial.h>

#include "nrf.h"
#include "nrf_gpio.h"
//#include "nRF51822.h"
#include "cfg/cs_Boards.h"

#define NRF_UART_9600_BAUD  0x00275000UL
#define NRF_UART_38400_BAUD 0x009D5000UL

/**
 * Configure the UART. Currently we set it on 38400 baud.
 */
void _config_uart(uint8_t pinRx, uint8_t pinTx) {
	// Disable UART
	NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Disabled;
	
	nrf_gpio_cfg_output(pinTx);
	nrf_gpio_cfg_input(pinRx, NRF_GPIO_PIN_NOPULL);

	// Configure UART pins
	NRF_UART0->PSELRXD = pinRx;
	NRF_UART0->BAUDRATE = NRF_UART_38400_BAUD;
	
	NRF_UART0->PSELTXD = pinTx;

	NRF_UART0->PSELRTS  = 0xFFFFFFFF;
	NRF_UART0->PSELCTS  = 0xFFFFFFFF;
	
	// Disable parity and interrupt
	NRF_UART0->CONFIG   = (UART_CONFIG_PARITY_Excluded  << UART_CONFIG_PARITY_Pos );
	NRF_UART0->CONFIG  |= (UART_CONFIG_HWFC_Disabled    << UART_CONFIG_HWFC_Pos   );

	//NRF_UART0->CONFIG = NRF_UART0->CONFIG_HWFC_ENABLED; // do not enable hardware flow control.
	
	
	// Re-enable the UART
	NRF_UART0->ENABLE           = UART_ENABLE_ENABLE_Enabled;
	NRF_UART0->INTENSET         = 0;
	NRF_UART0->TASKS_STARTTX    = 1;
	NRF_UART0->TASKS_STARTRX    = 1;
	
	//NRF_UART0->TASKS_STARTTX = 1;
	//NRF_UART0->TASKS_STARTRX = 1;
	//NRF_UART0->EVENTS_RXDRDY = 0;
	//NRF_UART0->EVENTS_TXDRDY = 0;
}

void _write_token(const char token) {
	NRF_UART0->TXD = (uint8_t)token;
	while(NRF_UART0->EVENTS_TXDRDY != 1) {}
	NRF_UART0->EVENTS_TXDRDY = 0;
}

void _write_string(const char *str, int len) {
	for(int i = 0; i < len; ++i) {
		NRF_UART0->TXD = (uint8_t)str[i];
		while(NRF_UART0->EVENTS_TXDRDY != 1) {}
		NRF_UART0->EVENTS_TXDRDY = 0;
	}
}

void _get_dec_str(char* str, uint32_t len, uint32_t val) {
	uint8_t i;
	for(i=1; i<=len; i++)
	{
		if (val == 0) {
			str[len-i] = '0';
		} else {
			str[len-i] = (uint8_t) ((val % 10UL) + '0');
			val/=10;
		}
	}
	str[i-1] = '\0';
}
