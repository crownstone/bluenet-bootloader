/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**@file
 *
 * @defgroup ble_sdk_app_bootloader_main main.c
 * @{
 * @ingroup dfu_bootloader_api
 * @brief Bootloader project main file.
 *
 * -# Receive start data package.
 * -# Based on start packet, prepare NVM area to store received data.
 * -# Receive data packet.
 * -# Validate data packet.
 * -# Write Data packet to NVM.
 * -# If not finished - Wait for next packet.
 * -# Receive stop data packet.
 * -# Activate Image, boot application.
 *
 */
#include "dfu.h"
#include "dfu_transport.h"
#include "bootloader.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "nordic_common.h"
#include "nrf.h"
#ifndef S310_STACK
#include "nrf_mbr.h"
#endif // S310_STACK
#ifdef S130
#include "nrf_mbr.h"
#endif
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "nrf51.h"
#include "ble_hci.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "app_gpiote.h"
#include "nrf_error.h"
// #include "boards.h"
#include "ble_debug_assert_handler.h"
#include "softdevice_handler.h"
#include "pstorage_platform.h"

#define SERIAL

#ifdef SERIAL
#include "serial.h"
#endif
#include "cs_Boards.h"

#ifndef SERIAL
static void stub0() {}
static void stub1(char * var, int var1) {}
#define write_string stub1
#define config_uart stub0
#endif

// forward declaration (is not present in softdevice_handler.h for now (remove if it gets added)
void softdevice_assertion_handler(uint32_t pc, uint16_t line_num, const uint8_t * file_name);

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                                       /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define APP_GPIOTE_MAX_USERS            1                                                       /**< Number of GPIOTE users in total. Used by button module and dfu_transport_serial module (flow control). */

#define APP_TIMER_PRESCALER             0                                                       /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            3 //2 doesn't matter                                                       /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                                       /**< Size of timer operation queues. */

//#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)                /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, 0)                        /**< Maximum size of scheduler events. */

#define SCHED_QUEUE_SIZE                20 //10 doesn't matter                                                      /**< Maximum number of events in the scheduler queue. */

#define	COMMAND_ENTER_RADIO_BOOTLOADER		66

/**@brief Function for error handling, which is called when an error has occurred.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name.
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
	//nrf_gpio_pin_set(LED_7);
	// This call can be used for debug purposes during application development.
	// @note CAUTION: Activating this code will write the stack to flash on an error.
	//                This function should NOT be used in a final product.
	//                It is intended STRICTLY for development/debugging purposes.
	//                The flash write will happen EVEN if the radio is active, thus interrupting
	//                any communication.
	//                Use with care. Un-comment the line below to use.
	// ble_debug_assert_handler(error_code, line_num, p_file_name);

	// On assert, the system can only recover on reset.
	volatile uint32_t error __attribute__((unused)) = error_code;
	volatile uint16_t line __attribute__((unused)) = line_num;
	volatile const uint8_t* file __attribute__((unused)) = p_file_name;
	__asm("BKPT");
	while(1) {}

	// NVIC_SystemReset();
}


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] file_name   File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
	app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

/**@brief Function for initializing the GPIOTE handler module.
*/
static void gpiote_init(void)
{
	APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
	// Initialize timer module, making it use the scheduler.
	APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, true);
}


/**@brief Function for initializing the button module.
*/
/*
	static void buttons_init(void)
	{
	nrf_gpio_cfg_sense_input(BOOTLOADER_BUTTON_PIN,
	BUTTON_PULL,
	NRF_GPIO_PIN_SENSE_LOW);
	}
	*/

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void sys_evt_dispatch(uint32_t event)
{
	pstorage_sys_event_handler(event);
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
	uint32_t err_code;

#ifndef S310_STACK
	sd_mbr_command_t com = {SD_MBR_COMMAND_INIT_SD, };
	err_code = sd_mbr_command(&com);
	APP_ERROR_CHECK(err_code);

	err_code = sd_softdevice_vector_table_base_set(BOOTLOADER_REGION_START);
	APP_ERROR_CHECK(err_code);
#endif // S310_STACK

#ifdef S130
	sd_mbr_command_t com = {SD_MBR_COMMAND_INIT_SD, };
	err_code = sd_mbr_command(&com);
	APP_ERROR_CHECK(err_code);

	err_code = sd_softdevice_vector_table_base_set(BOOTLOADER_REGION_START);
	APP_ERROR_CHECK(err_code);
#endif

#if(HARDWARE_BOARD == CROWNSTONE || HARDWARE_BOARD == CROWNSTONE2)
	SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, false);
#else
	SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, true);
#endif

#ifndef S310_STACK
	// Enable BLE stack
	ble_enable_params_t ble_enable_params;
	memset(&ble_enable_params, 0, sizeof(ble_enable_params));
	ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
	err_code = sd_ble_enable(&ble_enable_params);
	APP_ERROR_CHECK(err_code);
#endif // S310_STACK

	err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
	APP_ERROR_CHECK(err_code);
}

static void clk_init() {
	//start up crystal HF clock.
	NRF_CLOCK->TASKS_HFCLKSTART = 1;
	while(!NRF_CLOCK->EVENTS_HFCLKSTARTED);

	// generate clock, RC=0, XTAL=1, SYNTH=2
	NRF_CLOCK->LFCLKSRC = 2; // (CLOCK_LFCLKSRC_SRC_SYNTH << CLOCK_LFCLKSRC_SRC_Pos);
//#else
//	NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos);
//#endif
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_LFCLKSTART = 1;
	while(!NRF_CLOCK->EVENTS_LFCLKSTARTED);

	NRF_POWER->TASKS_CONSTLAT = 1;
}

/**@brief Function for event scheduler initialization.
*/
static void scheduler_init(void)
{
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


/**@brief Function for application main entry.
*/
int main(void)
{
	uint32_t err_code;
	//sd_mbr_command_t com = {SD_MBR_COMMAND_INIT_SD, };
	//err_code = sd_mbr_command(&com);

#ifdef S130
	clk_init();
#endif
	//bool     bootloader_is_pushed = false;

	APP_ERROR_CHECK_BOOL(*((uint32_t *)NRF_UICR_BOOT_START_ADDRESS) == BOOTLOADER_REGION_START);
	APP_ERROR_CHECK_BOOL(NRF_FICR->CODEPAGESIZE == CODE_PAGE_SIZE);

	// Initialize.
	timers_init();
	gpiote_init();
	config_uart();
	write_string("Firmware 0.1.0\r\n", 16);
	//buttons_init();
#if(HARDWARE_BOARD == CROWNSTONE || HARDWARE_BOARD == CROWNSTONE2)
	write_string("Init BLE stack\r\n", 16);
#endif
	ble_stack_init();
#if(HARDWARE_BOARD == CROWNSTONE || HARDWARE_BOARD == CROWNSTONE2)
	write_string("Init scheduler\r\n", 16);
#endif
	scheduler_init();

	bool manual_request = false;
	uint32_t gpregret;
	err_code = sd_power_gpregret_get(&gpregret);
	APP_ERROR_CHECK(err_code);
	if (gpregret == COMMAND_ENTER_RADIO_BOOTLOADER) {
		write_string("Manual request\r\n", 17);
		manual_request = true;
	} else if (gpregret == 0) {
		write_string("Accidental reboot\r\n", 20);
	} else {
		// just reset, do the same as with accidental reboot
		write_string("Manual reset\r\n", 15);
	}

	bool valid_app = bootloader_app_is_valid(DFU_BANK_0_REGION_START);
	if(!valid_app) {
		write_string("No valid app\r\n", 14);
	}
	// clear the register, so we don't end up all the time in the bootloader
	err_code = sd_power_gpregret_clr(0xFF);
	APP_ERROR_CHECK(err_code);

	if ((!valid_app) || manual_request) {
		write_string("Wait for new app!\r\n", 19);

		// Initiate an update of the firmware.
		err_code = bootloader_dfu_start();
		APP_ERROR_CHECK(err_code);
	} else {
		write_string("Load app\r\n", 10); // indicates app will be loaded

		// Select a bank region to use as application region.
		// @note: Only applications running from DFU_BANK_0_REGION_START is supported.
		//nrf_gpio_pin_clear(PIN_LED0); // indicates the bootloader stopped running
		bootloader_app_start(DFU_BANK_0_REGION_START);
	}

	NVIC_SystemReset();
}
