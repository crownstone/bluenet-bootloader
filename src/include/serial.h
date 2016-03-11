/**
 * Author: Anne van Rossum
 * Copyright: Distributed Organisms B.V. (DoBots)
 * Date: 10 Oct., 2014
 * License: LGPLv3+, Apache License, or MIT, your choice
 */

#ifndef CS_SERIAL_H
#define CS_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#undef DEBUG
#define DEBUG                0
#define INFO                 1
#define WARN                 2
#define ERROR                3
#define FATAL                4
#define NONE                 5

/**
 * General configuration of the serial connection. This sets the pin to be used for UART, the baudrate, the parity 
 * bits, etc.
 */
void _config_uart();

/**
 * Write a string to the serial connection. Make sure you end with `\n` if you want to have new lines in the output. 
 * Also flushing the buffer might be done around new lines.
 */
//void write(const char *str);

/**
 * Write a string with printf functionality.
 */
//int write(const char *str, ...);

void _write_token(const char token);
void _write_string(const char *str, int len);
void _get_dec_str(char* str, uint32_t len, uint32_t val);

void _stub0();
void _stub1(char * var, int var1);
void _stub2(char* str, uint32_t len, uint32_t val);

#if SERIAL_VERBOSITY<NONE
#define config_uart  _config_uart
#define write_string _write_string
#define get_dec_str  _get_dec_str
#else
#define config_uart  _stub0
#define write_string _stub1
#define get_dec_str  _stub2
#endif

#ifdef __cplusplus
}
#endif

#endif
