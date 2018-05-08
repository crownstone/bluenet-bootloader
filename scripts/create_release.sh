#!/bin/bash

#############################################################################
# Create a new release
#
# This will:
#  - Create a new CMakeBuild.config in $BLUENET_BOOTLOADER_DIR/release/model_version/
#  - Build bootloader, DFU package and docs and copy them to $BLUENET_RELEASE_DIR/bootloaders/
#  - Update the index.json file in $BLUENET_RELEASE_DIR to keep
#    track of stable, latest, and release dates (for cloud)
#  - Create a change log file from git commits since last release
#  - Create a git tag with the version number
#
#############################################################################

path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source $BLUENET_DIR/scripts/_utils.sh

############################
### Pre Script Verification
############################

if [ -z $BLUENET_RELEASE_DIR ]; then
	cs_err "BLUENET_RELEASE_DIR is not defined!"
	exit 1
fi
if [ -z $BLUENET_BOOTLOADER_DIR ]; then
	cs_err "BLUENET_BOOTLOADER_DIR is not defined!"
	exit 1
fi
BLUENET_RELEASE_CANDIDATE_DIR="${BLUENET_RELEASE_DIR}-candidate"


##############################################
### Check bluenet branch, changes, and remote
##############################################

pushd $BLUENET_DIR &> /dev/null

# Check current branch, releases should be made from master branch
branch=$(git symbolic-ref --short -q HEAD)

if [[ $branch != "master" ]]; then
	cs_err "Bluenet is currently on branch '"$branch"'"
	cs_err "Releases should be made from master branch"
	cs_err "Are you sure you want to continue? [y/N]"
	read branch_response
	if [[ ! $branch_response == "y" ]]; then
		cs_info "abort"
		exit 1
	fi
fi

# Check for modifications
modifications=$(git ls-files -m | wc -l)

if [[ $modifications != 0 ]]; then
	cs_err "There are modified files in your bluenet code"
	cs_err "Commit the files or stash them first!!"
	exit 1
fi

# Check for untracked files
untracked=$(git ls-files --others --exclude-standard | wc -l)

if [[ $untracked != 0 ]]; then
	cs_err "The following untracked files were found in the bluenet code"
	cs_err "Make sure you didn't forget to add anything important!"
	git ls-files --others --exclude-standard
	cs_info "Do you want to continue? [Y/n]"
	read untracked_response
	if [[ $untracked_response == "n" ]]; then
		exit 1
	fi
fi

# check for remote updates
git remote update

# if true then there are remote updates that need to be pulled first
if [ ! $(git rev-parse HEAD) = $(git ls-remote $(git rev-parse --abbrev-ref @{u} | sed 's/\// /g') | cut -f1) ]; then
	cs_err "There are remote updates that were not yet pulled"
	cs_err "Are you sure you want to continue? [y/N]"
	read update_response
	if [[ ! $update_response == "y" ]]; then
		cs_info "abort"
		exit 1
	fi
fi

popd &> /dev/null


##############################################
### Check bootloader branch, changes, and remote
##############################################

pushd $BLUENET_BOOTLOADER_DIR &> /dev/null

# Check current branch, releases should be made from master branch
branch=$(git symbolic-ref --short -q HEAD)

if [[ $branch != "master" ]]; then
	cs_err "Bootloader is on branch '"$branch"'"
	cs_err "Releases should be made from master branch"
	cs_err "Are you sure you want to continue? [y/N]"
	read branch_response
	if [[ ! $branch_response == "y" ]]; then
		cs_info "abort"
		exit 1
	fi
fi

# Check for modifications
modifications=$(git ls-files -m | wc -l)

if [[ $modifications != 0 ]]; then
	cs_err "There are modified files in your bootloader code"
	cs_err "Commit the files or stash them first!!"
	exit 1
fi

# Check for untracked files
untracked=$(git ls-files --others --exclude-standard | wc -l)

if [[ $untracked != 0 ]]; then
	cs_err "The following untracked files were found in the bootloader code"
	cs_err "Make sure you didn't forget to add anything important!"
	git ls-files --others --exclude-standard
	cs_info "Do you want to continue? [Y/n]"
	read untracked_response
	if [[ $untracked_response == "n" ]]; then
		exit 1
	fi
fi

# check for remote updates
git remote update

# if true then there are remote updates that need to be pulled first
if [ ! $(git rev-parse HEAD) = $(git ls-remote $(git rev-parse --abbrev-ref @{u} | sed 's/\// /g') | cut -f1) ]; then
	cs_err "There are remote updates that were not yet pulled"
	cs_err "Are you sure you want to continue? [y/N]"
	read update_response
	if [[ ! $update_response == "y" ]]; then
		cs_info "abort"
		exit 1
	fi
fi

popd &> /dev/null





############################
### Prepare
############################

cs_info "Stable version? [y/N]: "
read stable
if [[ $stable == "y" ]]; then
	stable=1
else
	stable=0
fi

# Get old version number
if [ -f $BLUENET_BOOTLOADER_DIR/VERSION ]; then
	#current_version_str=`grep -oP "#define\s+BOOTLOADER_VERSION\s+\S+" include/version.h | grep -oP "(?<=\")\S+(?=\")"`
	current_version_str=`cat $BLUENET_BOOTLOADER_DIR/VERSION`
	version_list=(`echo $current_version_str | tr '.' ' '`)
	v_major=${version_list[0]}
	v_minor=${version_list[1]}
	v_patch=${version_list[2]}
	cs_log "Current version: $current_version_str"
	# v_minor=$((v_minor + 1))
	# v_patch=0
	suggested_version="$v_major.$v_minor.$v_patch"
else
	suggested_version="1.0.0"
fi

valid=0
existing=0

# Ask for new version number
while [[ $valid == 0 ]]; do
	cs_info "Enter a version number [$suggested_version]:"
	read -e version
	if [[ $version == "" ]]; then
		version=$suggested_version
	fi

	if [[ $version =~ ^[0-9]{1,2}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
		model="bootloader"

		directory="${BLUENET_BOOTLOADER_DIR}/release/${model}_${version}"

		# If release candidate, find the dir that doesn't exist yet. Keep up the RC number
		rc_prefix="-RC"
		rc_count=0
		rc_str=""
		if [[ $stable == 0 ]]; then
			while true; do
				rc_str="${rc_prefix}${rc_count}"
				directory="${BLUENET_BOOTLOADER_DIR}/release/${model}_${version}${rc_str}"
				if [[ ! -d $directory ]]; then
					break
				fi
				((rc_count++))
			done
			version="${version}${rc_str}"
		fi

		if [ -d $directory ]; then
			cs_err "Version already exists, are you sure? [y/N]: "
			read version_response
			if [[ $version_response == "y" ]]; then
				existing=1
				valid=1
			fi
		else
			valid=1
		fi
	else
		cs_err "Version (${version}) does not match pattern"
	fi
done

if [[ $stable == 0 ]]; then
	cs_info "Release candidate ${rc_count} (version ${version}), is that correct? [Y/n]"
	read version_response
	if [[ $version_response == "n" ]]; then
		cs_info "abort"
		exit 1
	fi
fi



#####################################
### Create Config file and directory
#####################################

# create directory in bluenet with new config file. copy from default and open to edit

if [[ $existing == 0 ]]; then
	cs_log "Creating new directory: "$directory
	mkdir $directory &> /dev/null

	cp $BLUENET_DIR/conf/cmake/CMakeBuild.config.default $directory/CMakeBuild.config

###############################
### Fill Default Config Values
###############################

	if [[ $model == "guidestone" ]]; then
		sed -i "s/BLUETOOTH_NAME=\".*\"/BLUETOOTH_NAME=\"Guide\"/" $directory/CMakeBuild.config
		sed -i "s/DEFAULT_HARDWARE_BOARD=.*/DEFAULT_HARDWARE_BOARD=GUIDESTONE/" $directory/CMakeBuild.config
	else
		sed -i "s/BLUETOOTH_NAME=\".*\"/BLUETOOTH_NAME=\"Crown\"/" $directory/CMakeBuild.config
		sed -i "s/DEFAULT_HARDWARE_BOARD=.*/DEFAULT_HARDWARE_BOARD=ACR01B2C/" $directory/CMakeBuild.config
	fi

	sed -i "s/BOOTLOADER_VERSION=\".*\"/BOOTLOADER_VERSION=\"$version\"/" $directory/CMakeBuild.config

	sed -i "s/NRF51822_DIR=/#NRF51822_DIR=/" $directory/CMakeBuild.config
	sed -i "s/COMPILER_PATH=/#COMPILER_PATH=/" $directory/CMakeBuild.config

	sed -i "s/CROWNSTONE_SERVICE=.*/CROWNSTONE_SERVICE=1/" $directory/CMakeBuild.config
	sed -i "s/INDOOR_SERVICE=.*/INDOOR_SERVICE=0/" $directory/CMakeBuild.config
	sed -i "s/GENERAL_SERVICE=.*/GENERAL_SERVICE=0/" $directory/CMakeBuild.config
	sed -i "s/POWER_SERVICE=.*/POWER_SERVICE=0/" $directory/CMakeBuild.config
	sed -i "s/SCHEDULE_SERVICE=.*/SCHEDULE_SERVICE=0/" $directory/CMakeBuild.config

	sed -i "s/PERSISTENT_FLAGS_DISABLED=.*/PERSISTENT_FLAGS_DISABLED=0/" $directory/CMakeBuild.config
	sed -i "s/SERIAL_VERBOSITY=.*/SERIAL_VERBOSITY=SERIAL_BYTE_PROTOCOL_ONLY/" $directory/CMakeBuild.config
	sed -i "s/DEFAULT_OPERATION_MODE=.*/DEFAULT_OPERATION_MODE=OPERATION_MODE_SETUP/" $directory/CMakeBuild.config

	xdg-open $directory/CMakeBuild.config &> /dev/null

	if [[ $? != 0 ]]; then
		cs_info "Open $directory/CMakeBuild.config in to edit the config"
	fi

	cs_log "After editing the config file, press [ENTER] to continue"
	read
else
	cs_warn "Warn: Using existing configuration"
fi

########################
### Update release index
########################

# NOTE: do this before modifying the paths otherwise BLUENET_RELEASE_DIR will point the the subdirectory
#   but the index file is located in the root directory

# goto bluenet scripts dir
pushd $BLUENET_DIR/scripts &> /dev/null

cs_info "Update release index ..."
if [[ $stable == 1 ]]; then
	./update_release_index.py -t bootloader -v $version -s
else
	:
	# ./update_release_index.py -t $model -v $version
fi

checkError "Failed"
cs_succ "Copy DONE"

popd &> /dev/null

############################
###  modify paths
############################

export BLUENET_CONFIG_DIR=$directory
export BLUENET_BUILD_DIR=$BLUENET_BUILD_DIR/$model"_"$version
if [[ $stable == 0 ]]; then
	# if [ -z $BLUENET_RELEASE_CANDIDATE_DIR ]; then
	# 	cs_err "$BLUENET_RELEASE_CANDIDATE_DIR does not exist!"
	# 	exit 1
	# fi
	export BLUENET_RELEASE_DIR=$BLUENET_RELEASE_CANDIDATE_DIR
fi
export BLUENET_RELEASE_DIR=$BLUENET_RELEASE_DIR/bootloaders/$model"_"$version
export BLUENET_BIN_DIR=$BLUENET_RELEASE_DIR/bin

############################
### Run
############################

###################
### Config File
###################

cs_info "Copy configuration to release dir ..."
mkdir -p $BLUENET_RELEASE_DIR/config
cp $BLUENET_CONFIG_DIR/CMakeBuild.config $BLUENET_RELEASE_DIR/config
checkError
# Write down which git hash of bluenet was used.
git rev-parse HEAD > $BLUENET_RELEASE_DIR/config/bluenet-git-hash.txt
cs_succ "Copy DONE"

###################
### Softdevice
###################

cs_info "Build softdevice ..."
pushd $BLUENET_DIR/scripts &> /dev/null
./softdevice.sh build
checkError
popd &> /dev/null
cs_succ "Softdevice DONE"


###################
### Bootloader
###################

# Build bootloader
cs_info "Build bootloader ..."
pushd $BLUENET_BOOTLOADER_DIR/scripts &> /dev/null
./all.sh
checkError
popd &> /dev/null
cs_succ "Build DONE"


###################
### DFU
###################

cs_info "Create DFU package ..."
pushd $BLUENET_DIR/scripts &> /dev/null
./dfuGenPkg.py -b "$BLUENET_BIN_DIR/bootloader_dfu.hex" -o $model"_"$version
checkError
sha1sum "${BLUENET_BIN_DIR}/${model}_${version}.zip" | cut -f1 -d " " > "${BLUENET_BIN_DIR}/${model}_${version}.zip.sha1"
checkError
popd &> /dev/null
cs_succ "DFU DONE"


####################
### GIT release tag
####################

cs_info "Do you want to add a tag and commit to git? [Y/n]"
read git_response
if [[ $git_response == "n" ]]; then
	cs_info "abort"
	exit 1
fi



cs_info "Add release config"
pushd $BLUENET_BOOTLOADER_DIR &> /dev/null
git add $directory
git commit -m "Add release config for "$model"_"$version
popd &> /dev/null


if [[ $stable == 1 ]]; then
	cs_info "Create git commit for release"
	pushd $BLUENET_BOOTLOADER_DIR &> /dev/null
	echo $version > VERSION

	cs_log "Updating changes overview"
	if [[ $current_version_str ]]; then
		echo "Version $version:" > tmpfile
		git log --pretty=format:" - %s" "v$current_version_str"...HEAD >> tmpfile
		echo "" >> tmpfile
		echo "" >> tmpfile
		cat CHANGES >> tmpfile
		mv tmpfile CHANGES
	else
		echo "Version $version:" > CHANGES
		git log --pretty=format:" - %s" >> CHANGES
		echo "" >> CHANGES
		echo "" >> CHANGES
	fi

	cs_log "Add to git"
	git add VERSION CHANGES
	git commit -m "Version bump to $version"
	popd &> /dev/null
fi

cs_log "Create tag"
pushd $BLUENET_BOOTLOADER_DIR &> /dev/null
git tag -a -m "Tagging version $version" "v$version"
popd &> /dev/null
cs_succ "DONE. Created Release "$version

