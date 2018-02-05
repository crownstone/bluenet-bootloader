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

#define VERBOSE

#define SERIAL_BYTE_PROTOCOL_ONLY   5
#define SERIAL_READ_ONLY            6
#define SERIAL_NONE                 7

/**
 * General configuration of the serial connection. This sets the pin to be used for UART, the baudrate, the parity
 * bits, etc.
 */
void _config_uart(uint8_t pinRx, uint8_t pinTx);

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

#define WRITE_VERBOSE(str, len)

#if SERIAL_VERBOSITY<SERIAL_READ_ONLY
#define config_uart  _config_uart
#else
#define config_uart(pinRx, pinTx)
#endif

#if SERIAL_VERBOSITY<SERIAL_BYTE_PROTOCOL_ONLY
#define write_string _write_string
#define get_dec_str  _get_dec_str

#ifdef VERBOSE
#undef WRITE_VERBOSE
#define WRITE_VERBOSE(str, len) _write_string(str, len);
#endif

#else
#undef VERBOSE
#define write_string(str, len)
#define get_dec_str(str, len, val)
#endif

#ifdef __cplusplus
}
#endif

#endif
