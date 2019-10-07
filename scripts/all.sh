#!/bin/bash

compilation_mode=release
#compilation_mode=debug

path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${path}/_utils.sh

# do not overwrite environment variables for target release
# we set the variables manually in the create_release.sh script
if [[ $1 != "release" ]]; then
	source ${path}/_check_targets.sh $target
fi

# configure environment variables, load configuration files, check targets and
# assign serial_num from target
source ${path}/_config.sh

objcopy=${COMPILER_PATH}/bin/${COMPILER_TYPE}objcopy
objsize=${COMPILER_PATH}/bin/${COMPILER_TYPE}size

device_variant=xxaa

# create output and build directories
if [[ $1 == "release" ]]; then
	# for release target, use BLUENET_BUILD_DIR as build dir and
	# BLUENET_RELEASE_DIR as output dir
	if [ -z $BLUENET_RELEASE_DIR ]; then
		cs_err "BLUENET_RELEASE_DIR is not defined!"
		exit 1
	fi
	build_path=${BLUENET_BUILD_DIR}
	output_path=${BLUENET_RELEASE_DIR}
else
	# for all other targets, use BLUENET_BUILD_DIR/bootloader
	# as build dir and BLUENET_BIN_DIR as output dir
	# also update the BLUENET_BUILD_DIR environment variable so
	# the make file uses the correct path
	export BLUENET_BUILD_DIR=${BLUENET_BUILD_DIR}/bootloader
	build_path=${BLUENET_BUILD_DIR}
	output_path=${BLUENET_BIN_DIR}
fi

# create build directory
mkdir -p "$build_path"
# create ouptut directory
mkdir -p "$output_path"

cs_log "Go to \"${path}/../gcc\""
cd "${path}/../gcc"

#cp gcc_nrf51_bootloader_xxaa.ld.in gcc_nrf51_bootloader_xxaa.ld
#sed -i "s/@RAM_R1_BASE@/${RAM_R1_BASE}/" gcc_nrf51_bootloader_xxaa.ld
#sed -i "s/@RAM_APPLICATION_AMOUNT@/${RAM_APPLICATION_AMOUNT}/" gcc_nrf51_bootloader_xxaa.ld
#sed -i "s/@BOOTLOADER_START_ADDRESS@/${BOOTLOADER_START_ADDRESS}/" gcc_nrf51_bootloader_xxaa.ld
#sed -i "s/@BOOTLOADER_LENGTH@/${BOOTLOADER_LENGTH}/" gcc_nrf51_bootloader_xxaa.ld
cp dfu_gcc_nrf52.ld.in dfu_gcc_nrf52.ld
sed -i "s/@RAM_R1_BASE@/${RAM_R1_BASE}/" dfu_gcc_nrf52.ld
sed -i "s/@RAM_APPLICATION_AMOUNT@/${RAM_APPLICATION_AMOUNT}/" dfu_gcc_nrf52.ld
sed -i "s/@BOOTLOADER_START_ADDRESS@/${BOOTLOADER_START_ADDRESS}/" dfu_gcc_nrf52.ld
sed -i "s/@BOOTLOADER_LENGTH@/${BOOTLOADER_LENGTH}/" dfu_gcc_nrf52.ld

#make $compilation_mode
make nrf52832_xxaa_s132
checkError "Error building bootloader"

cd $build_path

# convert and/or rename to general names (.out is actually in .elf format)
cs_log "Convert binaries to formats convenient for inspection and upload"
#$objcopy -j .text -j .data -O binary ${prefix}_bootloader_${device_variant}.out bootloader.bin
#$objcopy -j .text -j .data -O ihex ${prefix}_bootloader_${device_variant}.out bootloader_dfu.hex
#cp ${prefix}_bootloader_${device_variant}.out bootloader.elf
#cp ${prefix}_bootloader_${device_variant}.hex bootloader.hex
$objcopy -j .text -j .data -O binary nrf52832_xxaa_s132.out bootloader.bin
$objcopy -j .text -j .data -O ihex nrf52832_xxaa_s132.out bootloader_dfu.hex
cp nrf52832_xxaa_s132.out bootloader.elf
cp nrf52832_xxaa_s132.hex bootloader.hex

$objsize bootloader.elf

# copy to target path
cs_log "Copy files to $output_path"
cp bootloader.bin $output_path
cp bootloader.elf $output_path
cp bootloader.hex $output_path
cp bootloader_dfu.hex $output_path
