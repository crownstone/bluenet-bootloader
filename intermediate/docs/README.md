## Intermediate bootloader

This intermediate bootloader makes sure the upgrade process of the Legacy Bootloader + Softdevice v2.0 to Secure Bootloader + Softdevice v6.1 happens in a fault tolerant way. Each of the following process is illustrated in the `intermediate/docs/process.pdf`.

### Stage 0 (Bootloader (<=) v1.3.0)
This current legacy bootloader is replaced by the next stage bootloader. In the later stages, the bootloader will not start the firmware app in case there exists one.

### Stage 1 (Bootloader v1.8.0)	
Present at address 0x79000 (same as the existing bootloader), this bootloader expects `Bootloader v1.9.0 package` via DFU process. Once, the new bootloader (v1.9.0) is received, it copies it to the address `0x70000` and sets the BOOTADDR in UICR region to the same. Address `0x70000` has been chosen to make sure it can accomodate 5 pages (20kB size of the bootloader) and doesn't interfere with the DFU Bank region. However, any other address below 0x70000 but above DFU bank (from 0x5000) can be chosen safely. The incoming bootloader at this point will just replace the current legacy bootloader present in the device residing at the address `0x79000`.

### Stage 2 Bootloader v1.9.0
Present at address `0x70000`, this bootloader expects a DFU Package with both `Softdevice v6.1 and New Secure bootloader v2.0.0`. Once this package is received, it copies the softdevice to its address (0x1000), and copies the bootloader to the final address `0x76000`. After the copy of the bootloader is done, it verifies if the copy was successful. With a successful copy, the `BOOT_ADDR` (aka. `BOOTLOADER_UICR`, address = 0x10001014) in the UICR region is set to the final address `0x76000` and initiates a reset. Upon reset the new bootloader (v2.0.0) takes the control along with the new softdevice v6.1.0.

The following table shows how the significance of each step with some appropriate details.

| # | Process   | Vulnerable | Time Taken | Failure Consequence | DFU Content |  DFU BL Addr. |
|---|-----------|------------|------------|---------------------|----------|--------------|
| 1 | The intermediate bootloader is copied to the same old address. | No | ~10-15 secs | Will fail to write the new bootloader | Intermediate Bootloader | 0x79000 |
| 2 | The same intermediate bootloader is sent through the DFU to write at 0x70000 | No       | ~10-15 secs | Will fail to write the new bootloader while the control is still at 0x79000 | Intermediate Bootloader | 0x70000 |
| 3 | The secure bootloader along with softdevice 6.1 is sent as DFU update | No       | ~10-15 secs | Will fail to write the new bootloader while the control is still at 0x79000 | Secure Bootloader + Softdevice 6.1 | 0x70000 |


The following table shows the significance of each step in the final stage.

| # | Process   | Vulnerable | Time Taken | Failure Consequence |  BOOTLOADER_UICR |
|---|-----------|------------|------------|---------------------|-------------------|
| A | The combined package of Softdevice 6.1 + Secure Bootloader is sent over as DFU. | No       | ~40 secs | Will corrupt the DFU data present in bank-1. A fresh copy would be copied over the next update as the bootloader at 0x70000 is still preserved. | 0x70000 |
| B | The new softdevice 6.1 is copied into the its region. | No       | ~3-4 secs | Will fail to copy the new softdevice corrupting the current softdevice, however the copy operation would be retried as the bootloader (except for the BLE) is still operational. |  0x70000 |
| C | The new secure bootloader is copied into the address 0x76000 | No       | < 500 ms | Will fail to copy the new bootloader, however the copy operation would be retried as the bootloader (except for the BLE) is still operational at 0x70000. |  0x70000 |
| D | The `BOOTLOADER_UICR` is set to 0x76000 | No       | < 100 ms | Will fail to transfer the control to the new bootloader, however the set operation would be retried as the bootloader (except for the BLE) is still operational at 0x70000. |  0x79000 |

The flow is explained clearly here in `intermediate/docs/flowchart.pdf`. The intermediate bootloader functioning is show in green box in the flowchart.

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