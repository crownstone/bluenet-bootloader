#!/bin/bash

BLUENET_BUILD_DIR=build
if [[ -e "$BLUENET_CONFIG_DIR/targets.sh" ]]; then
	. $BLUENET_CONFIG_DIR/targets.sh $target #adjusts target and sets serial_num
else
	if [[ $target != "crownstone" ]]; then
		echo "To use different targets, copy \$BLUNET_DIR/targets_template.sh to $BLUENET_CONFIG_DIR and rename to targets.sh"
		exit 1
	fi
fi
