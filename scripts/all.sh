#!/bin/sh

compilation_mode=release
compilation_mode=debug

objcopy=/opt/compiler/gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-objcopy
objsize=/opt/compiler/gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-size

prefix=dobots
device_variant=xxaa

output_path=~/myworkspace/ble/bluenet/build

echo "++ Go to ../nrf51822_v6.0.0 - GCC_Bootloader/Board/nrf6310/device_firmware_updates/bootloader - gcc - BLE/gcc"
cd "../nrf51822_v6.0.0 - GCC_Bootloader/Board/nrf6310/device_firmware_updates/bootloader - gcc - BLE/gcc"

make $compilation_mode

# this creates a _build directory
cd _build

# convert and/or rename to general names (.out is actually in .elf format)
echo "++ Convert binaries to formats convenient for inspection and upload"
$objcopy -j .text -j .data -O binary ${prefix}_bootloader_${device_variant}.out bootloader.bin
cp ${prefix}_bootloader_${device_variant}.out bootloader.elf
cp ${prefix}_bootloader_${device_variant}.hex bootloader.hex

$objsize bootloader.elf

# copy to target path
echo "++ Copy files to $output_path"
cp bootloader.bin $output_path
cp bootloader.elf $output_path 
cp bootloader.hex $output_path 
