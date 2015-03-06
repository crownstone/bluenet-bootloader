DFU-Bootloader-for-gcc-compiler
===============================

This project contains code examples of a the DFU bootloader modified to be built by gcc. 

Note that the bootloader address was set to `0x00034000` to make enough room for the build with debug option. If you build with release (or all) (that has optimization) you can move the bootloader up to have more room for your application. This setting is used for both the `S110` and the `S130`, although the former doesn't need so much elbow room.

Tested with:

* nRF51 SDK version 6.0.0
* S130 0.5 alpha
* nRF51822 Development Kit version 2.1.0 or later

Or with:

* nRF51 SDK version 6.0.0
* S110 v7
* nRF51822 Development Kit version 2.1.0 or later

The project may need modifications to work with other versions or other boards.

## About this project

This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty.

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub.

The application is built to be used with the official nRF51 SDK, that can be downloaded from https://www.nordicsemi.no, provided you have a product key for one of our kits.

Please post any questions about this project on https://devzone.nordicsemi.com.

## Changes to make it work with the S130

The S130 is not normally supported by this bootloader. There are quite some changes required to make it work.

The directory that you need is [device_firmware_updates/bootloader - gcc - BLE](nrf51-dfu-bootloader-for-gcc-compiler/nrf51822_v6.0.0 - GCC_Bootloader/Board/nrf6310/device_firmware_updates/bootloader - gcc - BLE). 

One of the first things to note, is that the `arm_startup_nrf51.s` file is not used for `gcc`. The file 
`$NORDIC_SDK/Source/templates/gcc/gcc_startup_nrf51.s` is used instead. Instead of adjust that file in the `Makefile`
we set `ASMFLAGS` to create reasonable size for stack and heap:

    MEMORY_LIMITATIONS=-D__STACK_SIZE=1024 -D__HEAP_SIZE=1024

Also to make sure everything fits in the limited space, we adjust:

    CFLAGS += -Os -flto -ffunction-sections -fdata-sections -fno-builtin
    LDFLAGS = --specs=nano.specs -lc -lnosys -Wl,--gc-sections

Note that `Os` might actually not be optimal for restricting memory usage. It is optimal with respect to space 
required.

There are some references to pin layouts for our custom boards at `DoBots`. Feel free to remove those. It basically 
sets the right LEDS to go on/off.

The SoftDevice we currently use has a path at `/opt/softdevices/s130_nrf51822_0.5.0-1.alpha_API/include/`. You'll have
to update it if your softdevice SDK resides at another path.

## Configuration

The configuration is described in `dfu_types.h`. Note that most of the changes for the `S130` series are similar to
the ones of the `S310` series, so I use `S310_STACK` as a define on compilation!

	#define CODE_REGION_START 0x0001C000  // start of application (SoftDevice S130 is big!)
	#define BOOTLOADER_REGION_START 0x00034000 // start of bootloader at end of memory

In the `main.c` you will see one macro:

	#define SERIAL

Disable that to get rid of the printing to uart.

## Flashing

If you use https://github.com/mrquincle/bluenet to write your binaries to your chip, you will have to use the following
sequence:

	./softdevice.sh all
	./firmware.sh all bootloader 

The latter writes also bit fields to make the binary use the bootloader with the `writebyte.sh` utility. The fastest way to reset those flags is to flash a new softdevice. Do not use `writebyte.sh` or `./firmware.sh all bootloader` without erasing that area first.

In the https://github.com/mrquincle/bluenet/scripts directory, there is also a script to upload over the air:

	./upload_over_the_air.sh

If you use the firmware in that repository, you can also find code on how to enter the bootloader from the application.

### Bugs

Make sure you do not have any breakpoints set in an attached debugger. It works fine, but you might be thinking at 
times: "What is it doing?" Attaching it is fine, but just remove any default breakpoints in for example `gdbinit` or
similar scripts.

#### Nordic bug

Then there is one real bug:

You will have to change the code in `$NORDIC_SDK/Source/ble/ble_services/ble_dfu.c`. This is described on the [Nordic Forums](https://devzone.nordicsemi.com/question/22199/s130-how-to-check-if-norification-is-enabled/).

    static uint32_t on_ctrl_pt_write(ble_dfu_t * p_dfu, ble_gatts_evt_write_t * p_ble_write_evt)
    {
        ble_gatts_rw_authorize_reply_params_t write_authorize_reply;                                                        
                                                                                                                          
        write_authorize_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;                                                        
                                                                                                                        
        bool configured = true;                                                                                             
    #ifdef S130_BUG_SQUASHED
        configured = is_cccd_configured(p_dfu);                                                                             
    #endif
                                                                                                                        
        if (!configured) 
		...

If it bug is squashed you can put the `is_cccd_configured` back in there again of course.

#### Obscure bug

There is also a problem with `sd_softdevice_disable` when called from the bootloader. Hence, I refrain from actually
disabling the softdevice and do so in the application code. This happens in `bootloader.c` and the function
`bootloader_app_start()`. You see how I even set these magic bytes, but whatever I do, there is some magic
disappearance of my running code under the debugger. I don't know what exactly happens there.

    #ifndef S130
       err_code = sd_softdevice_disable();
       APP_ERROR_CHECK(err_code);
    #endif

You see how I added `false` to this if clause for now.


