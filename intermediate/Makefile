PROJECT_NAME := dfu_dual_bank_ble_s132_pca10040

export OUTPUT_FILENAME
#MAKEFILE_NAME := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) ) 

TEMPLATE_PATH = $(SDK_PATH)/components/toolchain/gcc
# ifeq ($(OS),Windows_NT)
# include $(TEMPLATE_PATH)/Makefile.windows
# else
# include $(TEMPLATE_PATH)/Makefile.posix
# endif

MK := mkdir
RM := rm -rf

#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO := 
else
NO_ECHO := @
endif

GNU_INSTALL_ROOT = /home/naveen/Development/gcc-arm
GNU_PREFIX = arm-none-eabi
SDK_PATH = /home/naveen/Development/nRF-sdk-11

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
include $(BLUENET_DIR)/conf/cmake/CMakeBuild.config.default
ifneq ("$(wildcard $(BLUENET_DIR)/CMakeBuild.config.local)","")
include $(BLUENET_DIR)/CMakeBuild.config.local
endif
include $(BLUENET_CONFIG_DIR)/CMakeBuild.config

DEFAULT_HARDWARE_BOARD=ACR01B2C

VER_1_8 = "1.8.0"
VER_1_9 = "1.9.0"
BOOTADDR_1_8 = 0x79000
BOOTADDR_1_9 = 0x70000

DEFVER = $(VER_1_8)
DEFBOOTADDR = $(BOOTADDR_1_8)

ifeq ("$(VER)",$(VER_1_8))
BOOTADDR = $(BOOTADDR_1_8)
else ifeq ("$(VER)",$(VER_1_9))
BOOTADDR = $(BOOTADDR_1_9)
else
VER = $(DEFVER)
# $(info Defaulting to ADDR: $(DEFBOOTADDR))
BOOTADDR = $(DEFBOOTADDR)
endif

PACKAGE_NAME = $(PACKAGE_PREFIX)_v$(VER).zip

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
C_SOURCE_FILES += \
$(abspath ./src/bootloader.c) \
$(abspath ./src/dfu_dual_bank.c) \
$(abspath ./src/dfu_ble_svc.c) \
$(abspath ./src/main.c) \
$(abspath ./src/dev_info_service.c) \
$(abspath ./src/dfu_transport_ble.c) \
$(abspath $(SDK_PATH)/components/libraries/util/app_error.c) \
$(abspath $(SDK_PATH)/components/libraries/util/app_error_weak.c) \
$(abspath $(SDK_PATH)/components/libraries/scheduler/app_scheduler.c) \
$(abspath $(SDK_PATH)/components/libraries/timer/app_timer.c) \
$(abspath $(SDK_PATH)/components/libraries/timer/app_timer_appsh.c) \
$(abspath $(SDK_PATH)/components/libraries/util/app_util_platform.c) \
$(abspath $(SDK_PATH)/components/libraries/bootloader_dfu/bootloader_settings.c) \
$(abspath $(SDK_PATH)/components/libraries/bootloader_dfu/bootloader_util.c) \
$(abspath $(SDK_PATH)/components/libraries/crc16/crc16.c) \
$(abspath $(SDK_PATH)/components/libraries/bootloader_dfu/dfu_init_template.c) \
$(abspath $(SDK_PATH)/components/libraries/hci/hci_mem_pool.c) \
$(abspath $(SDK_PATH)/components/libraries/util/nrf_assert.c) \
$(abspath $(SDK_PATH)/components/drivers_nrf/delay/nrf_delay.c) \
$(abspath $(SDK_PATH)/components/drivers_nrf/common/nrf_drv_common.c) \
$(abspath $(SDK_PATH)/components/drivers_nrf/pstorage/pstorage_raw.c) \
$(abspath $(SDK_PATH)/components/ble/common/ble_advdata.c) \
$(abspath $(SDK_PATH)/components/ble/common/ble_conn_params.c) \
$(abspath $(SDK_PATH)/components/ble/ble_services/ble_dfu/ble_dfu.c) \
$(abspath $(SDK_PATH)/components/ble/ble_services/ble_dis/ble_dis.c) \
$(abspath $(SDK_PATH)/components/ble/common/ble_srv_common.c) \
$(abspath $(SDK_PATH)/components/toolchain/system_nrf52.c) \
$(abspath $(SDK_PATH)/components/softdevice/common/softdevice_handler/softdevice_handler.c) \
$(abspath $(SDK_PATH)/components/softdevice/common/softdevice_handler/softdevice_handler_appsh.c) \
$(abspath $(BLUENET_DIR)/src/cfg/cs_Boards.c) \

#assembly files common to all targets
ASM_SOURCE_FILES  = $(abspath $(SDK_PATH)/components/toolchain/gcc/gcc_startup_nrf52.s)

#includes common to all targets
INC_PATHS += -I$(abspath ./config)
INC_PATHS += -I$(abspath ./config/bsp)
INC_PATHS += -I$(abspath ./inc)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/libraries/scheduler)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/drivers_nrf/config)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/drivers_nrf/delay)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/libraries/crc16)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/softdevice/s132/headers/nrf52)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/libraries/util)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/ble/common)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/drivers_nrf/pstorage)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/libraries/bootloader_dfu/ble_transport)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/device)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/libraries/hci)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/libraries/timer)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/softdevice/s132/headers)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/toolchain/CMSIS/Include)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/drivers_nrf/hal)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/toolchain/gcc)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/toolchain)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/drivers_nrf/common)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/softdevice/common/softdevice_handler)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/ble/ble_services/ble_dfu)
INC_PATHS += -I$(abspath $(SDK_PATH)/components/ble/ble_services/ble_dis)
INC_PATHS += -I$(abspath $(BLUENET_DIR)/include)

OBJECT_DIRECTORY = _build
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

#flags common to all targets
CFLAGS  = -DBOOTLOADER_START_ADDRESS=$(BOOTADDR)
CFLAGS += -DNRF52
CFLAGS += -DBOOTLOADER_VERSION=\"$(VER)\"
CFLAGS += -DUICR_BOARD_INDEX=1
# CFLAGS += -DDEBUG_LEDS # This is the debug flag to use the LEDs for visual feedback of the each step.
CFLAGS += -DDEFAULT_HARDWARE_BOARD=$(DEFAULT_HARDWARE_BOARD)
CFLAGS += -DBOARD_PCA10040
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
LDFLAGS += -Wl,--defsym=BOOTLOADER_START_ADDRESS=$(BOOTADDR)

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DNRF52
ASMFLAGS += -DBOARD_PCA10040
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

#default target - first one defined
default: clean bootloader

#building all targets
all: clean
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e cleanobj
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e nrf52832_xxaa_s132

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	bootloader

C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.s=.o) )

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

bootloader: OUTPUT_FILENAME := bootloader
bootloader: LINKER_SCRIPT=./dfu_gcc_nrf52.ld

bootloader: details $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@cp $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).elf
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize

# Target to print precondition details
details:
	@echo Selecting bootloader version $(VER) for address $(BOOTADDR)

## Create build directories
$(BUILD_DIRECTORIES):
	echo $(MAKEFILE_NAME)
	$(MK) $@

# Create objects from C SRC files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

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

flash: bootloader
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex -f nrf52  --sectorerase
	nrfjprog --reset -f nrf52

flash_softdevice:
	@echo Flashing: s132_nrf52_2.0.0_softdevice.hex
	nrfjprog --program $(SDK_PATH)/components/softdevice/s132/hex/s132_nrf52_2.0.0_softdevice.hex -f nrf52 --chiperase
	nrfjprog --reset -f nrf52

package: bootloader
	@echo Creating a DFU package for $(VER)
	@nrfutil dfu genpkg --bootloader _build/bootloader.hex --sd-req 0x81 $(PACKAGE_NAME)
