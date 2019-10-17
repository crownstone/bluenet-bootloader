#!/bin/bash

release_dir=~/dev/bluenet-workspace-sdk15/release-candidate/bootloaders/
RC="-RC0"

release() {
	mkdir -p "$release_dir/bootloader_${1}${RC}/bin"
	echo 'cp bin_${1}/* "$release_dir/bootloader_${1}${RC}/bin"'
	cp bin_${1}/* "$release_dir/bootloader_${1}${RC}/bin"
	echo 'cp bootloader_v${1}.zip "$release_dir/bootloader_${1}${RC}/bin/bootloader_${1}${RC}.zip"'
	cp bootloader_v${1}.zip "$release_dir/bootloader_${1}${RC}/bin/bootloader_${1}${RC}.zip"
	sha1sum bootloader_v${1}.zip | cut -f1 -d " " > "$release_dir/bootloader_${1}${RC}/bin/bootloader_${1}${RC}.zip.sha1"
}

if [ ! -d "$release_dir" ]; then
	echo "Dir $release_dir does not exist"
	exit 1
fi

release 1.8.0
release 1.8.1
release 1.9.0
release 1.9.1
