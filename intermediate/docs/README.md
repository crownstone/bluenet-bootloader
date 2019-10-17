## Intermediate bootloader

This intermediate bootloader makes sure the upgrade process of the Legacy Bootloader + Softdevice v2.0 to Secure Bootloader + Softdevice v6.1 happens in a fault tolerant way. The following stages are visualized in the [process illustration](process.pdf).

### Stage 0: Bootloader (<=) v1.3.0
This current legacy bootloader is replaced by the next stage bootloader. In the later stages, the bootloader will not start the firmware app in case there exists one.

### Stage 1: Bootloader v1.8.0
Present at address `0x79000` (same as the old bootloader), this bootloader expects `Bootloader v1.9.0 package` via DFU process. Once, the new bootloader (v1.9.0) is received, it copies it to the address `0x71000` and sets the BOOTADDR in UICR region to the same.

### Stage 2: Bootloader v1.9.0
Present at address `0x71000`, this bootloader expects a DFU Package with both `Softdevice v6.1 and bootloader v2`. Once this package is received, it copies the softdevice to its address (`0x1000`), and copies the bootloader to the final address `0x76000`. After the copy of the bootloader is done, it verifies if the copy was successful. With a successful copy, the `BOOT_ADDR` (aka. `BOOTLOADER_UICR`, address = 0x10001014) in the UICR region is set to the final address `0x76000` and initiates a reset. Upon reset the new bootloader (v2) takes the control along with the new softdevice v6.1.0.

### Intermediate bootloader address
The largest address that the intermediate bootloader (which is 5 pages in size) can be at is `0x76000 - 0x5000 = 0x71000`, as to not overlap with the v2 bootloader.

In case app data should be preserved, the intermediate bootloader address should also be adjust as to not overlap the app data of the 1.8.0 bootloader.
So in case you have 0x4000 bytes of app data, the intermediate bootloader should be `0x79000 - 0x4000 - 0x5000 = 0x70000`.

Copying the app data from the old app data region to the new app data region (in this example from `0x79000 - 0x4000 = 0x75000` to `0x70000 - 0x4000 = 0x6C000`, and after that to `0x76000 - 0x4000 = 0x72000`) is not implemented yet though.

### Bank size
At first, we thought the bank size was too small, but the update process of SDK 11 is different than SDK 15. See [SDK 11](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v11.0.0/bledfu_memory_banks.html) vs [SDK 15](https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.3.0/lib_bootloader_dfu_banks.html) documentation.

This means, however that the uploaded softdevice (located at bank0, or `0x1C000`) overlaps with where it will be copied to. The bootloader solves this by copying the softdevice in 3 parts. In our case (old softdevice ends at `0x1C000`, new softdevice ends at `0x26000` and is of size `0x25000`) this becomes:

- Copy `0xD800` bytes from `0x1C000` to `0x1000`.
- Copy `0xD800` bytes from `0x29800` to `0xE800`.
- Copy `0xA000` bytes from `0x37000` to `0x1C000`.

This process is implemented in `dfu_sd_img_block_swap()`, used by `dfu_sd_image_swap()`. The values used in the above copies can be calculated via:

- `0xD800 = (0x1C000 - 0x1000) / 2`
- `0x29800 = 0x1C000 + 0xD800`
- `0xE800 = 0x1000 + 0xD800`
- `0xA000 = 0x26000 - 0x1C000`
- `0x37000 = 0x1C000 + 0x25000 - 0xA000`

### Reserved app data
For this upgrade, we don't need to preserve the app data. However, changing the reserved app data size, will change the bank1 address, leading to issues when the bootloader image gets validated. See this [post](https://devzone.nordicsemi.com/question/54242/updating-bootloader-to-bootloader-which-preserves-app-data/) and this [post](https://devzone.nordicsemi.com/question/81141/cant-update-bootlader-over-the-air-when-changing-dfu_app_data_reserved/).

This can be fixed as done [here](https://github.com/crownstone/bluenet-bootloader/blob/d868454874f8b2829c2fb6f92c8bba2fa1a3a1fb/src/dfu_dual_bank.c#L923), but for the intermediate bootloader, we just kept the reserved app data the same, as it doesn't matter anyway.

### Bootloader 1.8.0 and 1.9.0 flow
The program flow can be viewed in the [flowchart](flowchart.pdf).

## Build process:

### Requirements

* nRF SDK 11
* nRF SDK 15.3
* nRF Crownstone's New (Current) Builds (Firmware, Bootloader)
* `nrfutil` version `0.3.0`
* Create a file (named `paths.config`) in the same directory as `Makefile`. In this file, define the variables, `GNU_INSTALL_ROOT` to point to the path where `gcc-arm` is installed and `SDK_PATH` to top level of `nRF SDK 11` directory. This should looks as follows:
```
GNU_INSTALL_ROOT = /home/workspace/Development/gcc-arm
SDK_PATH = /home/workspace/Development/nRF-sdk-11
```

### Step 0

To create the intermediate version at once, run the following command `$ create.sh debug`. This will create two DFU packages namely, `debug_v1.8.0.zip` and `debug_v1.9.0.zip`. If this step is used, the first sub-step of both step 1 and 2 can be skipped.

### Step 1

* To create a bootloader of version 1.8.0, run the following command `$ create.sh debug 1.8.0`. This will create a DFU package of the version v1.8.0 with a package called `debug_v1.8.0.zip`
* DFU the above created package ONLY when the read firmware version is `1.3.0`.

### Step 2

* To create a bootloader of version 1.8.0, run the following command `$ create.sh debug 1.9.0`. This will create a DFU package of the version v1.8.0 with a package called `debug_v1.9.0.zip`
* DFU the above created package ONLY when the read firmware version is `1.8.0`.

### Step 3

With the penultimate stage completed.
* Create a DFU package for Softdevice 6.1, Crownstone's Bootloader (or Nordic's SDK 15.3 Secure bootloader), with the following command: `$ nrfutil dfu genpkg --bootloader <crownstone_bins_dir>/bootloader.hex --softdevice <softdevice_path/>s132_nrf52_6.1.1_softdevice.hex --sd-req 0x81 <package_name>.zip`.
* DFU the above created package ONLY when the read firmware version is `1.9.0`.

### File Changed with respect to bootloader v1.3.0
* src/bootloader.c
* src/dfu_dual_bank.c
* src/dfu_transport_ble.c
* src/main.c
* inc/dfu_types.h
* inc/dfu.h