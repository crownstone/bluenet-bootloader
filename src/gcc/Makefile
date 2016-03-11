# Retrieve config settings
ifndef BLUENET_CONFIG_DIR
$(error BLUENET_CONFIG_DIR is not set. Make sure it is set: not just BLUENET_DIR)
endif

ifndef BLUENET_DIR
$(error BLUENET_DIR is not set. Make sure it is set.)
endif

# TODO: is this optional this way?
include $(BLUENET_DIR)/CMakeBuild.config.default
include $(BLUENET_DIR)/CMakeBuild.config.local
include $(BLUENET_CONFIG_DIR)/CMakeBuild.config
# Check that file to know how to set HARDWARE_BOARD

ifndef HARDWARE_BOARD
$(error HARDWARE_BOARD is not set. Check BLUENET_CONFIG_DIR/CMakeBuild.config)
endif

CFLAGS += -DBLE_STACK_SUPPORT_REQD -DBOOTLOADER_BANKED -DBLE_STACK_SUPPORT_REQD -DS$(SOFTDEVICE_SERIES)
#CFLAGS += -DBLE_STACK_SUPPORT_REQD -DBOOTLOADER_BANKED -DBLE_STACK_SUPPORT_REQD

# Required by Nordic specific files
# CFLAGS += -DBOARD_NRF6310

# Defines from the config file
CFLAGS += -DSOFTDEVICE_SERIES=$(SOFTDEVICE_SERIES)
CFLAGS += -DSOFTDEVICE_MAJOR=$(SOFTDEVICE_MAJOR)
CFLAGS += -DSOFTDEVICE_MINOR=$(SOFTDEVICE_MINOR)
CFLAGS += -DAPPLICATION_START_ADDRESS=$(APPLICATION_START_ADDRESS)
CFLAGS += -DBOOTLOADER_START_ADDRESS=$(BOOTLOADER_START_ADDRESS)
CFLAGS += -DAPP_DATA_RESERVED=$(APP_DATA_RESERVED)
CFLAGS += -DSERIAL_VERBOSITY=$(SERIAL_VERBOSITY)

# For the ble_debug_assert_handler
CFLAGS += -DBSP_DEFINES_ONLY -DBSP_SIMPLE

# Required for writing to proper gpio pins
CFLAGS += -DHARDWARE_BOARD=$(HARDWARE_BOARD)
CFLAGS += -DHARDWARE_VERSION=$(HARDWARE_VERSION)

TARGET_CHIP := NRF51822_QFAA_CA
# BOARD := BOARD_NRF6310
BOARD := DUMMY
#ASMFLAGS += -D__HEAP_SIZE=16 -D__STACK_SIZE=2048
#ASMFLAGS += -D__HEAP_SIZE=2048 -D__STACK_SIZE=2048
ASMFLAGS += -D__HEAP_SIZE=0 -D__STACK_SIZE=2048

#MEMORY_LIMITATIONS=-D__STACK_SIZE=1024 -D__HEAP_SIZE=1024

DEVICESERIES := nrf51

SDK_PATH=$(NRF51822_DIR)/

OUTPUT_FILENAME=dobots

C_SOURCE_FILES += main.c
#C_SOURCE_FILES += dfu_dual_bank.c
C_SOURCE_FILES += dfu_single_bank.c
C_SOURCE_FILES += dfu_transport_ble.c
C_SOURCE_FILES += serial.c
C_SOURCE_FILES += bootloader.c
C_SOURCE_FILES += bootloader_util_gcc.c
C_SOURCE_FILES += dfu_init.c

#C_SOURCE_FILES += $(SDK_PATH)components/libraries/bootloader_dfu/dfu_dual_bank.c
#C_SOURCE_FILES += $(SDK_PATH)components/libraries/bootloader_dfu/dfu_transport_ble.c
#C_SOURCE_FILES += $(SDK_PATH)components/libraries/bootloader_dfu/bootloader.c
#C_SOURCE_FILES += $(SDK_PATH)components/libraries/bootloader_dfu/bootloader_util_gcc.c


C_SOURCE_FILES += $(SDK_PATH)components/ble/common/ble_conn_params.c
C_SOURCE_FILES += $(SDK_PATH)components/ble/common/ble_advdata.c
#C_SOURCE_FILES += $(SDK_PATH)components/ble/common/ble_srv_common.c
C_SOURCE_FILES += $(SDK_PATH)components/ble/ble_services/ble_dfu/ble_dfu.c
#C_SOURCE_FILES += $(SDK_PATH)components/ble/ble_debug_assert_handler/ble_debug_assert_handler.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/timer/app_timer.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/timer/app_timer_appsh.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/scheduler/app_scheduler.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/gpiote/app_gpiote.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/hci/hci_mem_pool.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/crc16/crc16.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/util/app_util_platform.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/util/app_error.c
#C_SOURCE_FILES += $(SDK_PATH)components/drivers_nrf/pstorage/pstorage.c
C_SOURCE_FILES += $(SDK_PATH)components/drivers_nrf/pstorage/pstorage_raw.c
C_SOURCE_FILES += $(SDK_PATH)components/softdevice/common/softdevice_handler/softdevice_handler.c
C_SOURCE_FILES += $(SDK_PATH)components/softdevice/common/softdevice_handler/softdevice_handler_appsh.c
C_SOURCE_FILES += $(SDK_PATH)components/drivers_nrf/nrf_delay.c



INCLUDEPATHS += -I"$(SDK_PATH)components"
INCLUDEPATHS += -I"$(SDK_PATH)components/sdk"
INCLUDEPATHS += -I"$(SDK_PATH)components/ble/common"
#INCLUDEPATHS += -I"$(SDK_PATH)components/ble/ble_debug_assert_handler"
INCLUDEPATHS += -I"$(SDK_PATH)components/ble/ble_services/ble_dfu"
#INCLUDEPATHS += -I"$(SDK_PATH)components/ble/ble_error_log"
INCLUDEPATHS += -I"$(SDK_PATH)components/app_common"
INCLUDEPATHS += -I"$(SDK_PATH)components/device"
INCLUDEPATHS += -I"$(SDK_PATH)components/toolchain"
INCLUDEPATHS += -I"$(SDK_PATH)components/toolchain/gcc"
INCLUDEPATHS += -I"$(SDK_PATH)components/libraries/util"
INCLUDEPATHS += -I"$(SDK_PATH)components/libraries/scheduler"
INCLUDEPATHS += -I"$(SDK_PATH)components/libraries/timer"
INCLUDEPATHS += -I"$(SDK_PATH)components/libraries/gpiote"
INCLUDEPATHS += -I"$(SDK_PATH)components/libraries/crc16"
INCLUDEPATHS += -I"$(SDK_PATH)components/libraries/hci"
INCLUDEPATHS += -I"$(SDK_PATH)components/drivers_nrf/hal"
INCLUDEPATHS += -I"$(SDK_PATH)components/drivers_nrf/pstorage"
INCLUDEPATHS += -I"$(SDK_PATH)components/drivers_nrf/ble_flash"
INCLUDEPATHS += -I"$(SDK_PATH)components/softdevice/common/softdevice_handler"

INCLUDEPATHS += -I"$(SOFTDEVICE_DIR)/$(SOFTDEVICE_DIR_API)/include"

INCLUDEPATHS += -I"../ble_transport"
INCLUDEPATHS += -I"../include"
INCLUDEPATHS += -I"../"

ifndef BLUENET_DIR
$(error BLUENET_DIR is not set. Make sure it is set!)
endif

INCLUDEPATHS += -I"$(BLUENET_DIR)/include/cfg"

$(info INCLUDEPATHS = [${INCLUDEPATHS}])
$(info C_SOURCE_FILES = [${C_SOURCE_FILES}])

DEVICE_VARIANT := xxaa
#DEVICE_VARIANT := xxab

USE_SOFTDEVICE := S$(SOFTDEVICE_SERIES)

LINKER_SCRIPT = gcc_$(DEVICESERIES)_bootloader_$(DEVICE_VARIANT).ld
OUTPUT_FILENAME := $(OUTPUT_FILENAME)_bootloader_$(DEVICE_VARIANT)

# Gets overwritten..
#GNU_INSTALL_ROOT ?= $(COMPILER_PATH)
#GNU_PREFIX ?= $(COMPILER_TYPE)


#include $(SDK_PATH)components/toolchain/gcc/Makefile.common
# Include this common makefile, so that we can set the compiler path
include Makefile.common
