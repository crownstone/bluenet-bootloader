#MAKEFILE_NAME := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) )

# Retrieve config settings
ifndef BLUENET_CONFIG_DIR
ifndef BLUENET_WORKSPACE_DIR
$(error neither BLUENET_WORKSPACE_DIR nor BLUENET_CONFIG_DIR is not set. Make sure at least one of the two is set.)
endif
ifdef BLUENET_WORKSPACE_DIR
export BLUENET_CONFIG_DIR=$(BLUENET_WORKSPACE_DIR)/config
endif
endif

ifndef BLUENET_DIR
ifndef BLUENET_WORKSPACE_DIR
$(error neither BLUENET_WORKSPACE_DIR nor BLUENET_DIR is not set. Make sure at least one of the two is set.)
endif
ifdef BLUENET_WORKSPACE_DIR
export BLUENET_DIR=$(BLUENET_WORKSPACE_DIR)/bluenet
endif
endif

# TODO: is this optional this way?
include $(BLUENET_DIR)/CMakeBuild.config.default
ifneq ("$(wildcard $(BLUENET_DIR)/CMakeBuild.config.local)","")
include $(BLUENET_DIR)/CMakeBuild.config.local
endif
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
CFLAGS += -DDEVICE_TYPE=$(DEVICE_TYPE)

# For the ble_debug_assert_handler
CFLAGS += -DBSP_DEFINES_ONLY -DBSP_SIMPLE

# Required for writing to proper gpio pins
CFLAGS += -DHARDWARE_BOARD=$(HARDWARE_BOARD)
CFLAGS += -DHARDWARE_VERSION=$(HARDWARE_VERSION)

SDK_PATH=$(NRF51822_DIR)/


PROJECT_NAME := dfu_dual_bank_ble_s132_pca10036

export OUTPUT_FILENAME


#TEMPLATE_PATH = ../../../../../../components/toolchain/gcc
TEMPLATE_PATH = $(SDK_PATH)components/toolchain/gcc
#include $(dir $(lastword $(MAKEFILE_LIST)))/Makefile.posix
include Makefile.posix

MK := mkdir
RM := rm -rf

VERBOSE := 1
#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO :=
else
NO_ECHO := @
endif

# Toolchain commands
CC              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc'
AS              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as'
AR              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar' -r
LD              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld'
NM              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm'
OBJDUMP         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump'
OBJCOPY         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy'
SIZE            := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size'

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

#source common to all targets
C_SOURCE_PATHS += ../src/
C_SOURCE_FILES += dfu_ble_svc.c
C_SOURCE_FILES += main.c

C_SOURCE_FILES += bootloader.c
C_SOURCE_FILES += bootloader_settings.c
C_SOURCE_FILES += bootloader_util.c
C_SOURCE_FILES += dfu_dual_bank.c
#C_SOURCE_FILES += dfu_single_bank.c
C_SOURCE_FILES += dfu_init.c
C_SOURCE_FILES += dfu_transport_ble.c
#C_SOURCE_FILES += dfu_transport_serial.c
C_SOURCE_FILES += serial.c
C_SOURCE_FILES += dev_info_service.c

#C_SOURCE_FILES += $(SDK_PATH)components/libraries/util/app_error.c
#C_SOURCE_FILES += $(SDK_PATH)components/libraries/util/app_error_weak.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/scheduler/app_scheduler.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/timer/app_timer.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/timer/app_timer_appsh.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/util/app_util_platform.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/crc16/crc16.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/hci/hci_mem_pool.c
C_SOURCE_FILES += $(SDK_PATH)components/libraries/util/nrf_assert.c
C_SOURCE_FILES += $(SDK_PATH)components/drivers_nrf/delay/nrf_delay.c
C_SOURCE_FILES += $(SDK_PATH)components/drivers_nrf/common/nrf_drv_common.c
C_SOURCE_FILES += $(SDK_PATH)components/drivers_nrf/pstorage/pstorage_raw.c
C_SOURCE_FILES += $(SDK_PATH)components/ble/common/ble_advdata.c
C_SOURCE_FILES += $(SDK_PATH)components/ble/common/ble_conn_params.c
C_SOURCE_FILES += $(SDK_PATH)components/ble/ble_services/ble_dfu/ble_dfu.c
C_SOURCE_FILES += $(SDK_PATH)components/ble/common/ble_srv_common.c
C_SOURCE_FILES += $(SDK_PATH)components/toolchain/system_nrf52.c
C_SOURCE_FILES += $(SDK_PATH)components/softdevice/common/softdevice_handler/softdevice_handler.c
C_SOURCE_FILES += $(SDK_PATH)components/softdevice/common/softdevice_handler/softdevice_handler_appsh.c
C_SOURCE_FILES += $(SDK_PATH)components/ble/ble_services/ble_dis/ble_dis.c


#assembly files common to all targets
ASM_SOURCE_FILES  = $(SDK_PATH)components/toolchain/gcc/gcc_startup_nrf52.s

INC_PATHS += -I"../include/"

#includes common to all targets
# /!\ MISSING FILE /!\ #
#INC_PATHS += -I$(abspath ../../../config/dfu_dual_bank_ble_s132_pca10036)

#INC_PATHS += -I../src/config

INC_PATHS += -I$(SDK_PATH)components/libraries/scheduler
INC_PATHS += -I$(SDK_PATH)components/drivers_nrf/config
#INC_PATHS += -I$(SDK_PATH)examples/bsp
INC_PATHS += -I$(SDK_PATH)components/drivers_nrf/delay
INC_PATHS += -I$(SDK_PATH)components/libraries/crc16
INC_PATHS += -I$(SDK_PATH)components/softdevice/s132/headers/nrf52
INC_PATHS += -I$(SDK_PATH)components/libraries/util
INC_PATHS += -I$(SDK_PATH)components/ble/common
INC_PATHS += -I$(SDK_PATH)components/drivers_nrf/pstorage
#INC_PATHS += -I$(SDK_PATH)components/libraries/bootloader_dfu/ble_transport
INC_PATHS += -I$(SDK_PATH)components/device
INC_PATHS += -I$(SDK_PATH)components/libraries/hci
INC_PATHS += -I$(SDK_PATH)components/libraries/timer
INC_PATHS += -I$(SDK_PATH)components/softdevice/s132/headers
INC_PATHS += -I$(SDK_PATH)components/toolchain/CMSIS/Include
INC_PATHS += -I$(SDK_PATH)components/drivers_nrf/hal
INC_PATHS += -I$(SDK_PATH)components/toolchain/gcc
INC_PATHS += -I$(SDK_PATH)components/toolchain
INC_PATHS += -I$(SDK_PATH)components/drivers_nrf/common
INC_PATHS += -I$(SDK_PATH)components/softdevice/common/softdevice_handler
INC_PATHS += -I$(SDK_PATH)components/ble/ble_services/ble_dfu
INC_PATHS += -I$(SDK_PATH)components/ble/ble_services/ble_dis

ifneq ($(and $(SOFTDEVICE_DIR),$(SOFTDEVICE_DIR_API)),)
INC_PATHS += -I"$(SOFTDEVICE_DIR)/$(SOFTDEVICE_DIR_API)"
else
INC_PATHS += -I"$(SDK_PATH)components/softdevice/s${SOFTDEVICE_SERIES}/headers"
endif

INC_PATHS += -I"$(BLUENET_DIR)/include/"

$(info INC_PATHS = [${INC_PATHS}])
$(info C_SOURCE_FILES = [${C_SOURCE_FILES}])
$(info CFLAGS = [${CFLAGS}])

# OBJECT_DIRECTORY = _build
OBJECT_DIRECTORY = $(BLUENET_BUILD_DIR)/bootloader
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )


CFLAGS += -DNRF52
#CFLAGS += -DBOARD_PCA10040
CFLAGS += -DNRF52_PAN_12
CFLAGS += -DNRF52_PAN_15
CFLAGS += -DNRF52_PAN_58
CFLAGS += -DNRF52_PAN_55
CFLAGS += -DNRF52_PAN_54
CFLAGS += -DNRF52_PAN_31
CFLAGS += -DNRF52_PAN_30
CFLAGS += -DNRF52_PAN_51
CFLAGS += -DNRF52_PAN_36
CFLAGS += -DNRF52_PAN_53
CFLAGS += -D__HEAP_SIZE=0
CFLAGS += -DS132
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRF52_PAN_20
CFLAGS += -DNRF52_PAN_64
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DNRF52_PAN_62
CFLAGS += -DNRF52_PAN_63
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs --std=gnu99
CFLAGS += -Wall -Werror -Os -g3
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums
# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

# Assembler flags for the pca10040 board
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DNRF52
#ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DNRF52_PAN_12
ASMFLAGS += -DNRF52_PAN_15
ASMFLAGS += -DNRF52_PAN_58
ASMFLAGS += -DNRF52_PAN_55
ASMFLAGS += -DNRF52_PAN_54
ASMFLAGS += -DNRF52_PAN_31
ASMFLAGS += -DNRF52_PAN_30
ASMFLAGS += -DNRF52_PAN_51
ASMFLAGS += -DNRF52_PAN_36
ASMFLAGS += -DNRF52_PAN_53
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DS132
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DBSP_DEFINES_ONLY
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DNRF52_PAN_20
ASMFLAGS += -DNRF52_PAN_64
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF52_PAN_62
ASMFLAGS += -DNRF52_PAN_63


##flags for the 10036 board
#CFLAGS += -DNRF52_PAN_16
#CFLAGS += -DNRF52_PAN_23
#CFLAGS += -DNRF52
#CFLAGS += -DNRF52_PAN_4
#CFLAGS += -DNRF52_PAN_7
#CFLAGS += -DNRF52_PAN_1
#CFLAGS += -DNRF52_PAN_3
#CFLAGS += -DNRF52_PAN_2
#CFLAGS += -DSOFTDEVICE_PRESENT
#CFLAGS += -DNRF52_PAN_9
#CFLAGS += -DNRF52_PAN_8
#CFLAGS += -DNRF52_PAN_11
#CFLAGS += -DNRF52_PAN_64
#CFLAGS += -DNRF52_PAN_39
#CFLAGS += -DNRF52_PAN_38
#CFLAGS += -DNRF52_PAN_15
#CFLAGS += -DNRF52_PAN_58
#CFLAGS += -DNRF52_PAN_17
#CFLAGS += -DNRF52_PAN_65
#CFLAGS += -DNRF52_PAN_33
#CFLAGS += -DNRF52_PAN_32
#CFLAGS += -DNRF52_PAN_30
#CFLAGS += -DNRF52_PAN_37
#CFLAGS += -DNRF52_PAN_36
#CFLAGS += -DNRF52_PAN_35
#CFLAGS += -DNRF52_PAN_34
#CFLAGS += -D__HEAP_SIZE=0
#CFLAGS += -DNRF52_PAN_10
#CFLAGS += -DS132
#CFLAGS += -DCONFIG_GPIO_AS_PINRESET
#CFLAGS += -DNRF52_PAN_12
#CFLAGS += -DBLE_STACK_SUPPORT_REQD
#CFLAGS += -DBSP_DEFINES_ONLY
#CFLAGS += -DNRF52_PAN_28
#CFLAGS += -DNRF52_PAN_29
#CFLAGS += -DSWI_DISABLE0
#CFLAGS += -DNRF52_PAN_20
#CFLAGS += -DBOARD_PCA10036
#CFLAGS += -DNRF52_PAN_24
#CFLAGS += -DNRF52_PAN_25
#CFLAGS += -DNRF52_PAN_26
#CFLAGS += -DNRF52_PAN_27
#CFLAGS += -DNRF52_PAN_42
#CFLAGS += -DNRF52_PAN_43
#CFLAGS += -DNRF52_PAN_40
#CFLAGS += -DNRF52_PAN_41
#CFLAGS += -DNRF52_PAN_46
#CFLAGS += -DNRF52_PAN_47
#CFLAGS += -DNRF52_PAN_44
#CFLAGS += -DNRF52_PAN_63
#CFLAGS += -DNRF52_PAN_48
#CFLAGS += -DNRF52_PAN_49
#CFLAGS += -mcpu=cortex-m4
#CFLAGS += -mthumb -mabi=aapcs --std=gnu99
#CFLAGS += -Wall -Werror
#CFLAGS += -Os -g3
##CFLAGS += -DDEBUG -g3 -O0
##CFLAGS += -DDEBUG -g3 -Os

#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
## keep every function in separate section. This will allow linker to dump unused functions
#CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
#CFLAGS += -fno-builtin --short-enums
## keep every function in separate section. This will allow linker to dump unused functions
#LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
#LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
#LDFLAGS += -mcpu=cortex-m4
#LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
## let linker to dump unused sections
#LDFLAGS += -Wl,--gc-sections
## use newlib in nano version
#LDFLAGS += --specs=nano.specs -lc -lnosys

## Assembler flags for pca10036
#ASMFLAGS += -x assembler-with-cpp
#ASMFLAGS += -DNRF52_PAN_16
#ASMFLAGS += -DNRF52_PAN_23
#ASMFLAGS += -DNRF52
#ASMFLAGS += -DNRF52_PAN_4
#ASMFLAGS += -DNRF52_PAN_7
#ASMFLAGS += -DNRF52_PAN_1
#ASMFLAGS += -DNRF52_PAN_3
#ASMFLAGS += -DNRF52_PAN_2
#ASMFLAGS += -DSOFTDEVICE_PRESENT
#ASMFLAGS += -DNRF52_PAN_9
#ASMFLAGS += -DNRF52_PAN_8
#ASMFLAGS += -DNRF52_PAN_11
#ASMFLAGS += -DNRF52_PAN_64
#ASMFLAGS += -DNRF52_PAN_39
#ASMFLAGS += -DNRF52_PAN_38
#ASMFLAGS += -DNRF52_PAN_15
#ASMFLAGS += -DNRF52_PAN_58
#ASMFLAGS += -DNRF52_PAN_17
#ASMFLAGS += -DNRF52_PAN_65
#ASMFLAGS += -DNRF52_PAN_33
#ASMFLAGS += -DNRF52_PAN_32
#ASMFLAGS += -DNRF52_PAN_30
#ASMFLAGS += -DNRF52_PAN_37
#ASMFLAGS += -DNRF52_PAN_36
#ASMFLAGS += -DNRF52_PAN_35
#ASMFLAGS += -DNRF52_PAN_34
#ASMFLAGS += -D__HEAP_SIZE=0
#ASMFLAGS += -DNRF52_PAN_10
#ASMFLAGS += -DS132
#ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
#ASMFLAGS += -DNRF52_PAN_12
#ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
#ASMFLAGS += -DBSP_DEFINES_ONLY
#ASMFLAGS += -DNRF52_PAN_28
#ASMFLAGS += -DNRF52_PAN_29
#ASMFLAGS += -DSWI_DISABLE0
#ASMFLAGS += -DNRF52_PAN_20
#ASMFLAGS += -DBOARD_PCA10036
#ASMFLAGS += -DNRF52_PAN_24
#ASMFLAGS += -DNRF52_PAN_25
#ASMFLAGS += -DNRF52_PAN_26
#ASMFLAGS += -DNRF52_PAN_27
#ASMFLAGS += -DNRF52_PAN_42
#ASMFLAGS += -DNRF52_PAN_43
#ASMFLAGS += -DNRF52_PAN_40
#ASMFLAGS += -DNRF52_PAN_41
#ASMFLAGS += -DNRF52_PAN_46
#ASMFLAGS += -DNRF52_PAN_47
#ASMFLAGS += -DNRF52_PAN_44
#ASMFLAGS += -DNRF52_PAN_63
#ASMFLAGS += -DNRF52_PAN_48
#ASMFLAGS += -DNRF52_PAN_49

#ASMFLAGS += -DDEBUG -g3 -O0

#default target - first one defined
default: clean nrf52832_xxaa_s132

#building all targets
all: clean
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e cleanobj
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e nrf52832_xxaa_s132

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	nrf52832_xxaa_s132

C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_PATHS += $(C_SOURCE_PATHS)
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.s=.o) )

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

DEPENDENCIES := $(OBJECTS:.o=.d)

nrf52832_xxaa_s132: OUTPUT_FILENAME := nrf52832_xxaa_s132
#nrf52832_xxaa_s132: LINKER_SCRIPT=../../../dfu_gcc_nrf52.ld
#nrf52832_xxaa_s132: LINKER_SCRIPT=$(SDK_PATH)examples/dfu/bootloader/dfu_gcc_nrf52.ld
nrf52832_xxaa_s132: LINKER_SCRIPT=dfu_gcc_nrf52.ld

nrf52832_xxaa_s132: clean $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize

## Create build directories
$(BUILD_DIRECTORIES):
	echo $(MAKEFILE_NAME)
	$(MK) $@

-include $(DEPENDENCIES)

# Create objects from C SRC files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -MMD -c -o $@ $<

# Assemble files
$(OBJECT_DIRECTORY)/%.o: %.s
	@echo Assembly file: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<
# Link
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

finalize: genbin genhex echosize

genbin:
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
genhex:
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
echosize:
	-@echo ''
	$(NO_ECHO)$(SIZE) $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ''

clean:
	$(RM) $(BUILD_DIRECTORIES)

cleanobj:
	$(RM) $(BUILD_DIRECTORIES)/*.o
flash: nrf52832_xxaa_s132
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex -f nrf52  --chiperase
	nrfjprog --reset -f nrf52

## Flash softdevice
