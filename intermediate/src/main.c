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
#include "bsp.h"
#include "softdevice_handler_appsh.h"
#include "pstorage_platform.h"
#include "nrf_mbr.h"
#include "nrf_log.h"
#include "dfu.h"
#include "nrf_delay.h"

/* BLUENET includes START */
#include "cfg/cs_Boards.h"
#include "cfg/cs_DeviceTypes.h"
/* BLUENET includes END */

#if BUTTONS_NUMBER < 1
#error "Not enough buttons on board"
#endif

#if LEDS_NUMBER < 1
#error "Not enough LEDs on board"
#endif

#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                                       /**< Include the service_changed characteristic. For DFU this should normally be the case. */

#define BOOTLOADER_BUTTON               BSP_BUTTON_3                                            /**< Button used to enter SW update mode. */
#define UPDATE_IN_PROGRESS_LED          BSP_LED_2                                               /**< Led used to indicate that DFU is active. */

#define APP_TIMER_PRESCALER             0                                                       /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                                       /**< Size of timer operation queues. */

#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, 0)                        /**< Maximum size of scheduler events. */

#define SCHED_QUEUE_SIZE                20                                                      /**< Maximum number of events in the scheduler queue. */

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



#ifdef DEBUG_LEDS
/**@brief Function for initialization of LEDs.
 */
static void leds_init(void)
{
    nrf_gpio_range_cfg_output(LED_START, LED_STOP);
    nrf_gpio_pins_set(LEDS_MASK);
}
#endif


/**@brief Function for initializing the timer handler module (app_timer).
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler.
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
}

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
    uint32_t         err_code;
    sd_mbr_command_t com = {SD_MBR_COMMAND_INIT_SD, };

    nrf_clock_lf_cfg_t clock_lf_cfg = {.source        = NRF_CLOCK_LF_SRC_RC,   \
                                       .rc_ctiv       = 16,                    \
                                       .rc_temp_ctiv  = 2,                     \
                                       .xtal_accuracy = 0};

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

static void gpio_init(boards_config_t* board)
{
	if (IS_CROWNSTONE(board->deviceType)) {
		// PWM pin
		nrf_gpio_cfg_output(board->pinGpioPwm);
		if (board->flags.pwmInverted) {
			nrf_gpio_pin_set(board->pinGpioPwm);
		} else {
			nrf_gpio_pin_clear(board->pinGpioPwm);
		}

		// Relay pins
		if (board->flags.hasRelay) {
			nrf_gpio_cfg_output(board->pinGpioRelayOff);
			nrf_gpio_pin_clear(board->pinGpioRelayOff);
			nrf_gpio_cfg_output(board->pinGpioRelayOn);
			nrf_gpio_pin_clear(board->pinGpioRelayOn);
		}
	}
}

static void check_board_uicr() {
	uint32_t hardwareBoard = NRF_UICR->CUSTOMER[UICR_BOARD_INDEX];
	if (hardwareBoard == 0xFFFFFFFF) {
		uint32_t* hardwareBoardAddress = (uint32_t*)(NRF_UICR->CUSTOMER) + UICR_BOARD_INDEX;
//		nrf_nvmc_write_word(hardwareBoardAddress, DEFAULT_HARDWARE_BOARD);

		// Enable write.
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}

		*hardwareBoardAddress = DEFAULT_HARDWARE_BOARD;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}

		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
	}
}

/**@brief Function for bootloader main entry.
 */
int main(void)
{
    uint32_t err_code, gpregret;
    bool     dfu_start = false;
    bool     app_reset = false;

    // This check ensures that the defined fields in the bootloader corresponds with actual
    // setting in the chip.
    APP_ERROR_CHECK_BOOL(*((uint32_t *)NRF_UICR_BOOT_START_ADDRESS) == BOOTLOADER_REGION_START);
    APP_ERROR_CHECK_BOOL(NRF_FICR->CODEPAGESIZE == CODE_PAGE_SIZE);

    boards_config_t board = {};
	configure_board(&board);

	gpio_init(&board);
	// config_uart(board.pinGpioRx, board.pinGpioTx);

	check_board_uicr();

    // Initialize.
    timers_init();

#ifdef DEBUG_LEDS
    leds_init();
    nrf_gpio_pin_dir_set(DFU_UPDATE_LED,NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(DFU_SD_UPDATE_LED,NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(DFU_COMPLETE_LED,NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(BOOTLOADER_STARTUP_LED,NRF_GPIO_PIN_DIR_OUTPUT);

    for (int i = 0; i < 5; i++)
    {
        nrf_gpio_pin_toggle(BOOTLOADER_STARTUP_LED);
        nrf_delay_ms(500);
    }
#endif

    (void)bootloader_init();

    gpregret = NRF_POWER->GPREGRET;

    NRF_POWER->GPREGRET = GPREGRET_DEFAULT;

    switch (gpregret) {

        case GPREGRET_BROWNOUT_RESET + 10:
        case GPREGRET_DFU_RESET: {
			dfu_start = true; // Enter bootloader
	        break;
		}
        case BOOTLOADER_DFU_START: {
			app_reset = true;
			break;
		}
        case GPREGRET_SOFT_RESET: {
            break;
        }
        default: {
            gpregret += 1;
            NRF_POWER->GPREGRET = gpregret;
        }
    }

    if (bootloader_dfu_sd_in_progress())
    {
#ifdef DEBUG_LEDS
        nrf_gpio_pin_clear(UPDATE_IN_PROGRESS_LED);
#endif
        err_code = bootloader_dfu_sd_update_continue();
        APP_ERROR_CHECK(err_code);

        ble_stack_init(!app_reset);
        scheduler_init();

        err_code = bootloader_dfu_sd_update_finalize();
        APP_ERROR_CHECK(err_code);
#ifdef DEBUG_LEDS
        nrf_gpio_pin_set(UPDATE_IN_PROGRESS_LED);
#endif
    }
    else
    {
        // If stack is present then continue initialization of bootloader.
        ble_stack_init(!app_reset);
        scheduler_init();
    }

    dfu_start = true;
    
    // Forcing the dfu to start, keep waiting for the next bootloader endlessly
    if (dfu_start)
    {
#ifdef DEBUG_LEDS
        nrf_gpio_pin_clear(UPDATE_IN_PROGRESS_LED);
#endif
        // Initiate an update of the firmware.
        err_code = bootloader_dfu_start();
        APP_ERROR_CHECK(err_code);
#ifdef DEBUG_LEDS
        for (int i = 0; i < 5; i++)
        {
            nrf_gpio_pin_toggle(DFU_COMPLETE_LED);
            nrf_delay_ms(500);
        }
        nrf_gpio_pin_set(UPDATE_IN_PROGRESS_LED);
#endif
    }

    // The app is supposed to start by now, but the intermediate
    // bootloader should override and perform its updates.

    NVIC_SystemReset();
}
