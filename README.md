DFU-Bootloader-for-gcc-compiler for nRF52
===============================

This project contains code examples of a the DFU bootloader modified to be built by gcc. 

Note: In release (or all) mode, the bootloader uses ~32K memory, so the bootloader address is set to `0x00038000`. To build and use the bootloader with debug options, the bootloader address has to be set to `0x00034000` to make enough room for the build with debug options. 

Tested with:

* nRF5 SDK version 11
* S132 2.0.0
* nRF52 Development Kit v1.1.0

The project may need modifications to work with other versions or other boards.

## About this project

This application was originally forked from https://github.com/NordicSemiconductor/nrf51-dfu-bootloader-for-gcc-compiler but has in the meantime undergone drastic changes. Thus, the code may not have much in common anymore with the original fork. But it has been kept in sync with the bootloader included in the Nordic SDK version 11.0.0.

In addition to the code base from Nordic, we have added additional checks to the bootloader to handle errors in the firmware. One of these checks is the use of the GPREGRET register which was originally used solely to determine booting into DFU vs booting into firmware.

### Firmware Security checks

We added checks for device resets. I.e., the GPREGRET register is incremented on every boot. As long as the value is under a certain threshold, the bootloader loads the firmware. Otherwise it goes into DFU mode. The firmware in turn, set's the value back to 0 upon successful Bluetooth connection.

This way we are able to handle device resets due to:
- Firmware hardfaults
- Watchdog resets
- Software resets
- Wrong firmware build

Note: this only applies for software resets. If the device is reset (hard reset or brownout) or turned off, the GPREGRET register is cleared.

To handle brownouts, we use the power-fail comparator of the softdevice to get power fail warnings, and cause a software reset before the device can reset due to brownout. Thus we can still keep track of the resets even in the case of a brownout.

## Configuration

The configuration is described in `dfu_types.h`. Furthermore, we make use of the config file from [bluenet](https://github.com/crownstone/bluenet)

## Dependency

The project depends on the [bluenet]https://github.com/crownstone/bluenet) firmware. I.e. it shares some of the configuration files of the bluenet code where the different boards are defined. That means you will need a correctly set up Bluenet build system in order to compile the bootloader. See the installation manual [here](https://github.com/crownstone/bluenet/blob/master/INSTALL.md) for step-by-step instructions if you haven't done that already, or use the `install.sh` script on the [crownstone-sdk](https://github.com/crownstone/crownstone-sdk#bluenet_lib_configs) repository to setup the build system automatically.

## Flashing

If you use [bluenet](https://github.com/crownstone/bluenet) to write your binaries to your chip, you will have to use the following sequence:

	./softdevice.sh all
	./firmware.sh bootloader 

The latter writes also bit fields to make the binary use the bootloader with the `writebyte.sh` utility. The fastest way to reset those flags is to flash a new softdevice. Do not use `writebyte.sh` or `./firmware.sh all bootloader` without erasing that area first.

### DFU over the air

To upload firmware over the air, the Nordic tools can be used. This can be the NRF Master Control Panel on Android, or the Nordic tools for Windows.
To upload from Linux, we provide tools in Python on the [ble-automator](https://github.com/crownstone/ble-automator) repository.

E.g. to upload the firmware over the air, use

    ./dfu.py -i hci0 -f /path/to/config/build/crownstone.hex -a <DEVICE MAC ADDRESS>.
    
or to upload the bootloader use

    ./dfu.py -i hci0 -B -f /path/to/config/build/bootloader_df.hex -a <DEVICE MAC ADDRESS>
    
Note the `-B` option in the second call which specifies that a bootloader is uploaded (vs firmware)

The ble-automator repository also provides a python script to create the Distribution packet (ZIP) required to upload firmware using the NRF Master Control Panel.

E.g. to create an application zip, use

    ./dfuGenPkg.py -a /path/to/config/build/crownstone.hex

which will create a file application.zip
    
or to create the bootloader zip, use

    ./dfuGenPkg.py -b /path/to/config/build/bootloader_dfu.hex

### Bugs

Make sure you do not have any breakpoints set in an attached debugger. It works fine, but you might be thinking at 
times: "What is it doing?" Attaching it is fine, but just remove any default breakpoints in for example `gdbinit` or
similar scripts.

## Copyrights

All copyrights belong to the original authors (except for its changes, which fall under the same license as the 
original one). See: https://github.com/NordicSemiconductor/nrf51-dfu-bootloader-for-gcc-compiler

