DFU-Bootloader-for-gcc-compiler
===============================

This project contains code examples of a the DFU bootloader modified to be built by gcc. 

Note that the bootloader address should be set to `0x00034000` to make enough room for the build with debug option. If you build with release (or all) (that has optimization) you can move the bootloader up to have more room for your application. This setting is used for both the `S110` and the `S130`, although the former doesn't need so much elbow room.

Tested with:

* nRF51 SDK version 8.1.1
* S130 1.0.0
* nRF51822 Development Kit version 2.1.0 or later

And with:

* nRF51 SDK version 8.1.1
* S110 8.0.0
* nRF51822 Development Kit version 2.1.0 or later

The project may need modifications to work with other versions or other boards.

## About this project

This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty.

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub.

The application is built to be used with the official nRF51 SDK, that can be downloaded from https://www.nordicsemi.no, provided you have a product key for one of our kits.

Please post any questions about this project on https://devzone.nordicsemi.com.

## Configuration

The configuration is described in `dfu_types.h`. Furthermore, we make use of the config file from [bluenet](https://github.com/dobots/bluenet)

## Flashing

If you use [bluenet](https://github.com/mrquincle/bluenet) to write your binaries to your chip, you will have to use the following
sequence:

	./softdevice.sh all
	./firmware.sh bootloader 

The latter writes also bit fields to make the binary use the bootloader with the `writebyte.sh` utility. The fastest way to reset those flags is to flash a new softdevice. Do not use `writebyte.sh` or `./firmware.sh all bootloader` without erasing that area first.

In the [scripts](https://github.com/mrquincle/bluenet/scripts) directory, there is also a script to upload over the air:

	./upload_over_the_air.sh

If you use the firmware in that repository, you can also find code on how to enter the bootloader from the application.

### Bugs

Make sure you do not have any breakpoints set in an attached debugger. It works fine, but you might be thinking at 
times: "What is it doing?" Attaching it is fine, but just remove any default breakpoints in for example `gdbinit` or
similar scripts.

## Copyrights

All copyrights belong to the original authors (except for its changes, which fall under the same license as the 
original one). See: https://github.com/NordicSemiconductor/nrf51-dfu-bootloader-for-gcc-compiler

