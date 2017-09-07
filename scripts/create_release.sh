#!/bin/bash

#############################################################################
# Create a new release
#
# This will:
#  - Build bootloader and copy to $BLUENET_RELEASE_DIR/bootloader_version/
#  - Update the index.json file in $BLUENET_RELEASE_DIR to keep
#    track of stable, latest, and release dates (for cloud)
#  - Create a Change Log file from git commits since last release
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

pushd $BLUENET_BOOTLOADER_DIR &> /dev/null

# check current branch, releases should be made from master branch
branch=$(git symbolic-ref --short -q HEAD)

if [[ $branch != "master" ]]; then
	cs_err "You are currently on branch '"$branch"'"
	cs_err "Releases should be made from master branch"
	cs_err "Are you sure you want to continue? [y/N]"
	read branch_response
	if [[ ! $branch_response == "y" ]]; then
		cs_info "abort"
		exit 1
	fi
fi

# check for modifications in bluenet code
modifications=$(git ls-files -m | wc -l)

if [[ $modifications != 0 ]]; then
	cs_err "There are modified files in your code"
	cs_err "Commit the files or stash them first!!"
	exit 1
fi

# check for untracked files
untracked=$(git ls-files --others --exclude-standard | wc -l)

if [[ $untracked != 0 ]]; then
	cs_err "The following untracked files were found in the code"
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

# check version number
# enter version number?

valid=0
existing=0

# Get old version number
if [ -f $BLUENET_BOOTLOADER_DIR/VERSION ]; then
	version_str=`cat $BLUENET_BOOTLOADER_DIR/VERSION`
	version_list=(`echo $version_str | tr '.' ' '`)
	v_major=${version_list[0]}
	v_minor=${version_list[1]}
	v_patch=${version_list[2]}
	cs_log "Current version: $version_str"
	v_minor=$((v_minor + 1))
	v_patch=0
	suggested_version="$v_major.$v_minor.$v_patch"
else
	suggested_version="1.0.0"
fi

# Ask for new version number
while [[ $valid == 0 ]]; do
	cs_info "Enter a version number [$suggested_version]:"
	read -e version
	if [[ $version == "" ]]; then
		version=$suggested_version
	fi

	if [[ $version =~ ^[0-9]{1,2}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then

		# Check if version already exists
		directory=$BLUENET_RELEASE_DIR/"bootloader_"$version
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
		cs_err "Version does not match pattern"
	fi
done

#####################################
### Create Config file and directory
#####################################

# create directory in bluenet with new config file. copy from default and open to edit

if [[ $existing == 0 ]]; then
	cs_log "Creating new directory: "$directory
	mkdir $directory &> /dev/null
fi

cp $BLUENET_DIR/conf/cmake/CMakeBuild.config.default $directory/CMakeBuild.config


###############################
### Fill Default Config Values
###############################

# sed -i "s/FIRMWARE_VERSION=\".*\"/FIRMWARE_VERSION=\"$version\"/" $directory/CMakeBuild.config
# sed -i "s/DEVICE_TYPE=.*/DEVICE_TYPE=$device_type/" $directory/CMakeBuild.config

sed -i "s/NRF51822_DIR=/#NRF51822_DIR=/" $directory/CMakeBuild.config
sed -i "s/COMPILER_PATH=/#COMPILER_PATH=/" $directory/CMakeBuild.config

sed -i "s/CROWNSTONE_SERVICE=.*/CROWNSTONE_SERVICE=1/" $directory/CMakeBuild.config
sed -i "s/INDOOR_SERVICE=.*/INDOOR_SERVICE=0/" $directory/CMakeBuild.config
sed -i "s/GENERAL_SERVICE=.*/GENERAL_SERVICE=0/" $directory/CMakeBuild.config
sed -i "s/POWER_SERVICE=.*/POWER_SERVICE=0/" $directory/CMakeBuild.config
sed -i "s/SCHEDULE_SERVICE=.*/SCHEDULE_SERVICE=0/" $directory/CMakeBuild.config

sed -i "s/PERSISTENT_FLAGS_DISABLED=.*/PERSISTENT_FLAGS_DISABLED=0/" $directory/CMakeBuild.config
sed -i "s/BLUETOOTH_NAME=\".*\"/BLUETOOTH_NAME=\"Crown\"/" $directory/CMakeBuild.config
sed -i "s/SERIAL_VERBOSITY=.*/SERIAL_VERBOSITY=SERIAL_NONE/" $directory/CMakeBuild.config
sed -i "s/DEFAULT_OPERATION_MODE=.*/DEFAULT_OPERATION_MODE=OPERATION_MODE_SETUP/" $directory/CMakeBuild.config

xdg-open $directory/CMakeBuild.config &> /dev/null
if [[ $? != 0 ]]; then
	cs_info "Open $directory/CMakeBuild.config in to edit the config"
fi

cs_log "After editing the config file, press [ENTER] to continue"
read
#else
#	cs_warn "Warn: Using existing configuration"
#fi


########################
### Update release index
########################

cs_info "Stable version? [Y/n]: "
read stable
if [[ $stable == "n" ]]; then
	stable=0
else
	stable=1
fi

# NOTE: do this before modifying the paths otherwise BLUENET_RELEASE_DIR will point to the subdirectory
#   but the index file is located in the root directory

# goto bluenet scripts dir
pushd $BLUENET_DIR/scripts &> /dev/null

cs_info "Update release index ..."
if [[ $stable == 1 ]]; then
	./update_release_index.py -t "bootloader" -v $version -s
else
	./update_release_index.py -t "bootloader" -v $version
fi

checkError "Failed"
cs_succ "Copy DONE"

popd &> /dev/null

############################
###  modify paths
############################

export BLUENET_CONFIG_DIR=$directory
export BLUENET_BUILD_DIR=$BLUENET_BUILD_DIR/"bootloader_"$version
export BLUENET_RELEASE_DIR=$BLUENET_RELEASE_DIR/bootloaders/"bootloader_"$version
export BLUENET_BIN_DIR=$BLUENET_RELEASE_DIR/bin

############################
### Run
############################

###################
### Config File
###################

#create new config directory in release directory
mkdir -p $BLUENET_RELEASE_DIR/config

cs_info "Copy configuration to release dir ..."
cp $BLUENET_CONFIG_DIR/CMakeBuild.config $BLUENET_RELEASE_DIR/config

checkError
cs_succ "Copy DONE"

###################
### Bootloader
###################

cs_info "Updating version ..."

# update version
sed -i "s/BOOTLOADER_VERSION= \".*\"/BOOTLOADER_VERSION= \"$version\"/" $BLUENET_BOOTLOADER_DIR/include/version.h


pushd $BLUENET_BOOTLOADER_DIR/scripts &> /dev/null

# goto bootloader scripts dir

cs_info "Build bootloader ..."
#./all.sh release
./all.sh

checkError
cs_succ "Build DONE"

popd &> /dev/null

###################
### DFU
###################

# goto bluenet scripts dir
pushd $BLUENET_DIR/scripts &> /dev/null

./dfuGenPkg.py -b "$BLUENET_BIN_DIR/bootloader_dfu.hex" -o "bootloader_"$version

checkError
cs_succ "DFU DONE"

popd &> /dev/null

####################
### GIT release tag
####################

pushd $BLUENET_BOOTLOADER_DIR &> /dev/null

cs_info "Create git tag for release"

# if old version existed
if [[ $version_str ]]; then
	echo $version > VERSION

	cs_log "Updating changes overview"
	echo "Version $version:" > tmpfile
	git log --pretty=format:" - %s" "v$version_str"...HEAD >> tmpfile
	echo "" >> tmpfile
	echo "" >> tmpfile
	cat CHANGES >> tmpfile
	mv tmpfile CHANGES

	cs_log "Add to git"
	git add CHANGES VERSION "include/version.h"
	git commit -m "Version bump to $version"

	cs_log "Create tag"
	git tag -a -m "Tagging version $version" "v$version"

	# cs_log "Push tag"
	# git push origin --tags
else
	# setup first time
	echo $version > VERSION

	cs_log "Creating changes overview"
    git log --pretty=format:" - %s" >> CHANGES
    echo "" >> CHANGES
    echo "" >> CHANGES

	cs_log "Add to git"
    git add VERSION CHANGES
    git commit -m "Added VERSION and CHANGES files, Version bump to $version"

	cs_log "Create tag"
    git tag -a -m "Tagging version $version" "v$version"

	# cs_log "Push tag"
    # git push origin --tags
fi

cs_succ "DONE. Created Release bootloader"_$version

popd &> /dev/null
