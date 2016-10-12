#!/bin/bash
#
# check if _targets.sh is present in the BLUENET_CONFIG_DIR. See $BLUENET_DIR/_targets_template.sh for explanations

if [[ -z $1 ]]; then
	# if no target provided, set default target to crownstone
	target="crownstone"
else
	target=$1
fi

if [[ -e "$BLUENET_CONFIG_DIR/_targets.sh" ]]; then
	. $BLUENET_CONFIG_DIR/_targets.sh $target #adjusts target and sets serial_num
else
	if [[ $target != "crownstone" ]]; then
		echo "To use different targets, copy \$BLUNET_DIR/_targets_template.sh to $BLUENET_CONFIG_DIR and rename to _targets.sh"
		exit 1
	fi
fi
