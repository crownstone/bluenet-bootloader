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
 * -# Receive start data packet.
 * -# Based on start packet, prepare NVM area to store received data.
 * -# Receive data packet.
 * -# Validate data packet.
 * -# Write Data packet to NVM.
 * -# If not finished - Wait for next packet.
 * -# Receive stop data packet.
 * -# Activate Image, boot application.
 *
 */
#include "nrf_drv_config.h"
#include "dfu_transport.h"
#include "bootloader.h"
#include "bootloader_util.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "ble.h"
#include "nrf.h"
#include "ble_hci.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_error.h"
//#include "bsp.h"
#include "softdevice_handler_appsh.h"
#include "pstorage_platform.h"
#include "nrf_mbr.h"
//#include "nrf_log.h"

#include "version.h"
#include "serial.h"

/* BLUENET includes START */
#include "cfg/cs_Boards.h"
#include "cfg/cs_DeviceTypes.h"
/* BLUENET includes END */

#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                                       /**< Include the service_changed characteristic. For DFU this should normally be the case. */

//#define APP_GPIOTE_MAX_USERS            1                                                       /**< Number of GPIOTE users in total. Used by button module and dfu_transport_serial module (flow control). */

#define APP_TIMER_PRESCALER             0                                                       /**< Value of the RTC1 PRESCALER register. */
//#define APP_TIMER_MAX_TIMERS            3 //2 doesn't matter                                                       /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                                       /**< Size of timer operation queues. */

//#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)                /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, 0)                        /**< Maximum size of scheduler events. */

/* Maximum size of scheduler events. */
/*
//#define SCHED_MAX_EVENT_DATA_SIZE       ((CEIL_DIV(MAX(MAX(BLE_STACK_EVT_MSG_BUF_SIZE,        \
                                                           ANT_STACK_EVT_STRUCT_SIZE),        \
                                                           SYS_EVT_MSG_BUF_SIZE),             \
                                                           sizeof(uint32_t))) * sizeof(uint32_t))
*/

#define SCHED_QUEUE_SIZE                20 //10 doesn't matter                                                      /**< Maximum number of events in the scheduler queue. */

// command to enter dfu mode
#define GPREGRET_DFU_RESET              66
// command for normal reset
#define GPREGRET_SOFT_RESET             0
// gpregret value after dfu upload (or timeout)
#define GPREGRET_NEW_FIRMWARE_LOADED    64
// gpregret default value (to detect accidental resets)
#define GPREGRET_DEFAULT                1
// msk for brownout reset
#define GPREGRET_BROWNOUT_RESET         96

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
	//ble_debug_assert_handler(error_code, line_num, p_file_name);

	// On assert, the system can only recover on reset.
	volatile uint32_t error __attribute__((unused)) = error_code;
	volatile uint16_t line __attribute__((unused)) = line_num;
	volatile const uint8_t* file __attribute__((unused)) = p_file_name;
	__asm("BKPT");
	while(1) {}

	// NVIC_SystemReset();
}

/*lint -save -e14 */
void app_error_handler_bare(ret_code_t error_code)
{
	volatile uint32_t error __attribute__((unused)) = error_code;
	volatile uint16_t line __attribute__((unused)) = 0;
	volatile const uint8_t* file __attribute__((unused)) = NULL;
	__asm("BKPT");
	while(1) {}
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


/**@brief Function for initialization of LEDs.
 */
/*
static void leds_init(void)
{
    //nrf_gpio_cfg_output(UPDATE_IN_PROGRESS_LED);
    //nrf_gpio_pin_set(UPDATE_IN_PROGRESS_LED);
}
*/

/**@brief Function for initializing the timer handler module (app_timer).
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler.
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
}


/**@brief Function for initializing the button module.
 */
/*
static void buttons_init(void)
{
    nrf_gpio_cfg_sense_input(BOOTLOADER_BUTTON,
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
 *
 * @param[in] init_softdevice  true if SoftDevice should be initialized. The SoftDevice must only
 *                             be initialized if a chip reset has occured. Soft reset from
 *                             application must not reinitialize the SoftDevice.
 */
static void ble_stack_init(bool init_softdevice)
{
    uint32_t err_code;
    sd_mbr_command_t com = {SD_MBR_COMMAND_INIT_SD, };

	// Use recommended setting:
	// This will ensure calibration at least once every 8 seconds and for temperature changes of 0.5 degrees Celsius every 4 seconds.
    nrf_clock_lf_cfg_t clock_lf_cfg = {.source        = NRF_CLOCK_LF_SRC_RC,   \
                                       .rc_ctiv       = 16,                    \
                                       .rc_temp_ctiv  = 2,                     \
                                       .xtal_accuracy = 0};


/*
nrf_clock_lf_cfg_t clock_lf_cfg = {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM};
*/
//	nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC_RC_250_PPM_TEMP_8000MS_CALIBRATION;

    if (init_softdevice)
    {
        err_code = sd_mbr_command(&com);
        APP_ERROR_CHECK(err_code);
    }

    err_code = sd_softdevice_vector_table_base_set(BOOTLOADER_REGION_START);
    APP_ERROR_CHECK(err_code);

	SOFTDEVICE_HANDLER_APPSH_INIT(&clock_lf_cfg, true);

	// Enable BLE stack.
    ble_enable_params_t ble_enable_params;
//	memset(&ble_enable_params, 0, sizeof(ble_enable_params));
//	// Below code line is needed for s130. For s110 is inrrelevant - but executable
//	// can run with both s130 and s110.
//	ble_enable_params.gatts_enable_params.attr_tab_size   = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
	// Only one connection as a central is used when performing dfu.
	err_code = softdevice_enable_get_default_config(1, 1, &ble_enable_params);
	APP_ERROR_CHECK(err_code);

    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
	err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for event scheduler initialization.
*/
static void scheduler_init(void)
{
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


static void gpio_init(void)
{
#if IS_CROWNSTONE(DEVICE_TYPE)
	//! PWM pin
	nrf_gpio_cfg_output(PIN_GPIO_SWITCH);
#ifdef SWITCH_INVERSED
	nrf_gpio_pin_set(PIN_GPIO_SWITCH);
#else
	nrf_gpio_pin_clear(PIN_GPIO_SWITCH);
#endif

	//! Relay pins
#if HAS_RELAY
	nrf_gpio_cfg_output(PIN_GPIO_RELAY_OFF);
	nrf_gpio_pin_clear(PIN_GPIO_RELAY_OFF);
	nrf_gpio_cfg_output(PIN_GPIO_RELAY_ON);
	nrf_gpio_pin_clear(PIN_GPIO_RELAY_ON);
#endif
#endif
}

/**@brief Function for bootloader main entry.
*/
int main(void)
{
	uint32_t err_code;
	bool     dfu_start = false;
//	bool     app_reset = (NRF_POWER->GPREGRET == BOOTLOADER_DFU_START);
	bool     app_reset = false;
	uint32_t gpregret;

	// This check ensures that the defined fields in the bootloader corresponds with actual
	// setting in the chip.
	APP_ERROR_CHECK_BOOL(*((uint32_t *)NRF_UICR_BOOT_START_ADDRESS) == BOOTLOADER_REGION_START);
	APP_ERROR_CHECK_BOOL(NRF_FICR->CODEPAGESIZE == CODE_PAGE_SIZE);

	// Initialize.
	gpio_init();
	timers_init();
	config_uart();
	write_string("\r\nFirmware ", 11);
	write_string(BOOTLOADER_VERSION, strlen(BOOTLOADER_VERSION));
	write_string("\r\n", 2);
	(void)bootloader_init();

//	write_string("DFU_APP_DATA_RESERVED: ", 23);
//	char reservedText[6] = {0};
//	get_dec_str(reservedText, 5, DFU_APP_DATA_RESERVED);
//	write_string(reservedText, 6);
//	write_string("\r\n", 3);

	// get reset value from register
	gpregret = NRF_POWER->GPREGRET;

#ifdef VERBOSE
	char gpregretText[5] = {0};
	get_dec_str(gpregretText, 4, gpregret);
	WRITE_VERBOSE("gpregret=", 10);
	WRITE_VERBOSE(gpregretText, 5);
	WRITE_VERBOSE("\r\n", 3);

	char resetreasText[5] = {0};
	get_dec_str(resetreasText, 4, NRF_POWER->RESETREAS);
	WRITE_VERBOSE("resetreas=", 11);
	WRITE_VERBOSE(resetreasText, 5);
	WRITE_VERBOSE("\r\n", 3);
#endif

	// set register to default value (1_. if firmware is resetting accidentally, we
	// can start increasing this value.
	// note: if device is hard reset (power off, brownout reset pin) the register is cleared
	NRF_POWER->GPREGRET = GPREGRET_DEFAULT;

	switch(gpregret) {
		case GPREGRET_BROWNOUT_RESET + 10:
			write_string("Too many brownouts detected!!\r\n", 31);
		case GPREGRET_DFU_RESET: {
			write_string("Enter bootloader\r\n", 18);
			dfu_start = true;
	        break;
		}
		case BOOTLOADER_DFU_START: {
			// do we ever get here?
			write_string("Restarted bootloader\r\n", 22);
			app_reset = true;
			break;
		}
		case GPREGRET_SOFT_RESET: {
			write_string("Normal reboot\r\n", 15);
			break;
		}
		default: {
			// accidental reboot, start counting
			gpregret += 1;
			NRF_POWER->GPREGRET = gpregret;

#ifdef VERBOSE
			char gpregretText[5] = {0};
			get_dec_str(gpregretText, 4, gpregret);
			write_string("App reset, count=", 17);
			write_string(gpregretText, 5);
			WRITE_VERBOSE("\r\n", 3);
#endif
			break;
		}
	}

	if (bootloader_dfu_sd_in_progress())
	{
		write_string("Resume dfu\r\n", 12);

		err_code = bootloader_dfu_sd_update_continue();
		APP_ERROR_CHECK(err_code);

		WRITE_VERBOSE("Init BLE stack\r\n", 16);
		ble_stack_init(!app_reset);
		WRITE_VERBOSE("Init scheduler\r\n", 16);
		scheduler_init();

		err_code = bootloader_dfu_sd_update_finalize();
		APP_ERROR_CHECK(err_code);
	}
	else
	{
		// If stack is present then continue initialization of bootloader.
		WRITE_VERBOSE("Init BLE stack\r\n", 16);
		ble_stack_init(!app_reset);
		WRITE_VERBOSE("Init scheduler\r\n", 16);
		scheduler_init();
	}

	if (dfu_start || (!bootloader_app_is_valid(DFU_BANK_0_REGION_START))) {
		write_string("Start DFU\r\n", 12);
		// Initiate an update of the firmware.
		err_code = bootloader_dfu_start();
//#ifdef VERBOSE
//	char errText[5] = {0};
//	get_dec_str(errText, 4, err_code);
//	WRITE_VERBOSE("err_code=", 10);
//	WRITE_VERBOSE(errText, 5);
//	WRITE_VERBOSE("\r\n", 3);
//#endif
		APP_ERROR_CHECK(err_code);

		// set register to new value, if firmware is buggy, do a couple of resets, then
		// go back to dfu mode
		// note: this also happens if dfu times out
		// note2: [24.10.16] have to use the sd_power_gpregret functions after initializing the softdevice
		// writing to the register directly at this point will deadlock
		sd_power_gpregret_clr(0xFF);
		sd_power_gpregret_set(GPREGRET_NEW_FIRMWARE_LOADED);
	}

	if (bootloader_app_is_valid(DFU_BANK_0_REGION_START) && !bootloader_dfu_sd_in_progress())
	{
		write_string("Load app\r\n", 10);

		// Select a bank region to use as application region.
		// @note: Only applications running from DFU_BANK_0_REGION_START is supported.
		bootloader_app_start(DFU_BANK_0_REGION_START);
	}

	NVIC_SystemReset();
}
