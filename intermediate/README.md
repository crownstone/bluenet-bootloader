## Intermediate bootloader

This intermediate bootloader makes sure the upgrade process of the Legacy Bootloader + Softdevice v2.0 to Secure Bootloader + Softdevice v6.1 happens in a fault tolerant way.

For the current bootloaders present in the devices currently have a problem of not erasing the bootloader settings page after the DFU process has finished. This causes the new bootloader to start writing the stale content present in the DFU bank into an important region. If this problem is left unattended, it WILL instantly brick the device. To circumvent the problem, `GPREGRET` register has been used. To enter the bootloader copy process, it check for a `1` in the `GPREGRET` register. This is written when bootloader successfully flashes into its bank. This check process is disabled for stages after `1`.

### Stage 1
The incoming bootloader at this point will just replace the current legacy bootloader present in the device residing at the address `0x79000`.

### Stage 2
During the execution of this bootloader, it expects the DFU to contain only the bootloader for the address `0x70000`. The incoming bootloader is written to the address `0x70000` and handed-off the control to it.

### Stage 3
In this final stage of transition, the DFU is expected have **Secure bootloader and Softdevice 6.1**. This bootloader first writes the softdevice to the location where the SD resides. Later, the secure bootloader is copied to the final location `0x76000` and the `BOOTLOADER_UICR` is set to this new address and a reset is performed.

The following table shows how the significance of each step with some appropriate details.
| # | Process   | Vulnerable | Time Taken | Failure Consequence | DFU Content |  DFU BL Addr. |
|---|-----------|------------|------------|---------------------|-------------------|-------------------|
| 1 | The intermediate bootloader is copied to the same old address. | No       | ~10-15 secs | Will fail to write the new bootloader | Intermediate Bootloader | 0x79000 |
| 2 | The same intermediate bootloader is sent through the DFU to write at 0x70000 | No       | ~10-15 secs | Will fail to write the new bootloader while the control is still at 0x79000 | Intermediate Bootloader | 0x70000 |
| 3 | The secure bootloader along with softdevice 6.1 is sent as DFU update | No       | ~10-15 secs | Will fail to write the new bootloader while the control is still at 0x79000 | Secure Bootloader + Softdevice 6.1 | 0x70000 |


The following table shows how the significance of each step in the final stage.
| # | Process   | Vulnerable | Time Taken | Failure Consequence |  BOOTLOADER_UICR |
|---|-----------|------------|------------|---------------------|-------------------|-------------------|
| A | The combined package of Softdevice 6.1 + Secure Bootloader is sent over as DFU. | No       | ~40 secs | Will corrupt the DFU data present in bank-1. A fresh copy would be copied over the next update as the bootloader at 0x70000 is still preserved. | 0x70000 |
| B | This step is obsolete | No       | - | - | - |
| C | The new softdevice 6.1 is copied into the its region. | No       | ~3-4 secs | Will fail to copy the new softdevice corrupting the current softdevice, however the copy operation would be retried as the bootloader (except for the BLE) is still operational. |  0x70000 |
| D | The new secure bootloader is copied into the address 0x76000 | No       | < 500 ms | Will fail to copy the new bootloader, however the copy operation would be retried as the bootloader (except for the BLE) is still operational at 0x70000. |  0x70000 |
| E | The `BOOTLOADER_UICR` is set to 0x76000 | No       | < 100 ms | Will fail to transfer the control to the new bootloader, however the set operation would be retried as the bootloader (except for the BLE) is still operational at 0x70000. |  0x79000 |