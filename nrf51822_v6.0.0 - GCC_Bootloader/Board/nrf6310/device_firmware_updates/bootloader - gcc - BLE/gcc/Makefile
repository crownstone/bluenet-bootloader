# Retrieve config settings
ifndef BLUENET_CONFIG_DIR
$(error BLUENET_CONFIG_DIR is not set. Make sure it is set: not just BLUENET_DIR)
endif

include $(BLUENET_CONFIG_DIR)/CMakeBuild.config
# Check that file to know how to set HARDWARE_BOARD

ifndef HARDWARE_BOARD
$(error HARDWARE_BOARD is not set. Check BLUENET_CONFIG_DIR/CMakeBuild.config)
endif

CFLAGS += -DBLE_STACK_SUPPORT_REQD -DBOOTLOADER_BANKED -DBLE_STACK_SUPPORT_REQD -DS$(SOFTDEVICE_SERIES)

# Required by Nordic specific files
# CFLAGS += -DBOARD_NRF6310

# Required for writing to proper gpio pins
CFLAGS += -DHARDWARE_BOARD=$(HARDWARE_BOARD)
CFLAGS += -DHARDWARE_VERSION=$(HARDWARE_VERSION)

# Required to address the softdevice properly
#ifeq ($(SOFTDEVICE_SERIES),130)
#	CFLAGS += -DS310_STACK
#endif

TARGET_CHIP := NRF51822_QFAA_CA
# BOARD := BOARD_NRF6310
BOARD := DUMMY
ASMFLAGS += -D__HEAP_SIZE=16 -D__STACK_SIZE=2048

DEVICESERIES := nrf51

SDK_PATH=$(NRF51822_DIR)/

OUTPUT_FILENAME=dobots

C_SOURCE_FILES += main.c
C_SOURCE_FILES += dfu_dual_bank.c
C_SOURCE_FILES += dfu_transport_ble.c
C_SOURCE_FILES += serial.c
C_SOURCE_FILES += bootloader.c
C_SOURCE_FILES += bootloader_util_gcc.c

C_SOURCE_FILES += $(SDK_PATH)components/ble/common/ble_conn_params.c
C_SOURCE_FILES += $(SDK_PATH)components/ble/common/ble_advdata.c
#C_SOURCE_FILES += $(SDK_PATH)components/ble/common/ble_srv_common.c
C_SOURCE_FILES += $(SDK_PATH)components/ble/ble_services/ble_dfu/ble_dfu.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/timer/app_timer.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/scheduler/app_scheduler.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/gpiote/app_gpiote.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/hci/hci_mem_pool.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/crc16/crc16.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/util/app_util_platform.c
C_SOURCE_FILES += $(SDK_PATH)components/drivers_nrf/pstorage/pstorage.c
C_SOURCE_FILES += $(SDK_PATH)components/softdevice/common/softdevice_handler/softdevice_handler.c



INCLUDEPATHS += -I"$(SDK_PATH)components"
INCLUDEPATHS += -I"$(SDK_PATH)components/sdk"
INCLUDEPATHS += -I"$(SDK_PATH)components/ble/common"
INCLUDEPATHS += -I"$(SDK_PATH)components/ble/ble_debug_assert_handler"
INCLUDEPATHS += -I"$(SDK_PATH)components/ble/ble_services/ble_dfu"
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

INCLUDEPATHS += -I"../include/ble_transport"
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
include ../../../../../Source/templates/gcc/Makefile.common
