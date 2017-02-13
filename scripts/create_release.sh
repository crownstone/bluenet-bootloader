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
	err "BLUENET_RELEASE_DIR is not defined!"
	exit 1
fi
if [ -z $BLUENET_BOOTLOADER_DIR ]; then
	err "BLUENET_BOOTLOADER_DIR is not defined!"
	exit 1
fi

pushd $BLUENET_BOOTLOADER_DIR &> /dev/null

# check current branch, releases should be made from master branch
branch=$(git symbolic-ref --short -q HEAD)

if [[ $branch != "master" ]]; then
	err "You are currently on branch '"$branch"'"
	err "Releases should be made from master branch"
	err "Are you sure you want to continue? [y/N]"
	read branch_response
	if [[ ! $branch_response == "y" ]]; then
		info "abort"
		exit 1
	fi
fi

# check for modifications in bluenet code
modifications=$(git ls-files -m | wc -l)

if [[ $modifications != 0 ]]; then
	err "There are modified files in your code"
	err "Commit the files or stash them first!!"
	exit 1
fi

# check for untracked files
untracked=$(git ls-files --others --exclude-standard | wc -l)

if [[ $untracked != 0 ]]; then
	err "The following untracked files were found in the code"
	err "Make sure you didn't forget to add anything important!"
	git ls-files --others --exclude-standard
	info "Do you want to continue? [Y/n]"
	read untracked_response
	if [[ $untracked_response == "n" ]]; then
		exit 1
	fi
fi

# check for remote updates
git remote update

# if true then there are remote updates that need to be pulled first
if [ ! $(git rev-parse HEAD) = $(git ls-remote $(git rev-parse --abbrev-ref @{u} | sed 's/\// /g') | cut -f1) ]; then
	err "There are remote updates that were not yet pulled"
	err "Are you sure you want to continue? [y/N]"
	read update_response
	if [[ ! $update_response == "y" ]]; then
		info "abort"
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

# Get old version number
if [ -f $BLUENET_BOOTLOADER_DIR/VERSION ]; then
	version_str=`cat $BLUENET_BOOTLOADER_DIR/VERSION`
	version_list=(`echo $version_str | tr '.' ' '`)
	v_major=${version_list[0]}
	v_minor=${version_list[1]}
	v_patch=${version_list[2]}
	log "Current version: $version_str"
	v_minor=$((v_minor + 1))
	v_patch=0
	suggested_version="$v_major.$v_minor.$v_patch"
else
	suggested_version="1.0.0"
fi

# Ask for new version number
while [[ $valid == 0 ]]; do
	info "Enter a version number [$suggested_version]:"
	read -e version
	if [[ $version == "" ]]; then
		version=$suggested_version
	fi

	if [[ $version =~ ^[0-9]{1,2}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
		valid=1
	else
		err "Version does not match pattern"
	fi
done

########################
### Update release index
########################

info "Stable version? [Y/n]: "
read stable
if [[ $stable == "n" ]]; then
	stable=0
else
	stable=1
fi

# NOTE: do this before modifying the paths otherwise BLUENET_RELEASE_DIR will point the the subdirectory
#   but the index file is located in the root directory

# goto bluenet scripts dir
pushd $BLUENET_DIR/scripts &> /dev/null

info "Update release index ..."
if [[ $stable == 1 ]]; then
	./update_release_index.py -t "bootloader" -v $version -s
else
	./update_release_index.py -t "bootloader" -v $version
fi

checkError "Failed"
succ "Copy DONE"

popd &> /dev/null

############################
###  modify paths
############################

export BLUENET_BUILD_DIR=$BLUENET_BUILD_DIR/"bootloader_"$version
export BLUENET_RELEASE_DIR=$BLUENET_RELEASE_DIR/"bootloader_"$version
export BLUENET_BIN_DIR=$BLUENET_RELEASE_DIR

############################
### Run
############################

info "Updating version ..."

# update version
sed -i "s/BOOTLOADER_VERSION= \".*\"/BOOTLOADER_VERSION= \"$version\"/" $BLUENET_BOOTLOADER_DIR/include/version.h


pushd $BLUENET_BOOTLOADER_DIR/scripts &> /dev/null

# goto bootloader scripts dir

info "Build bootloader ..."
./all.sh release

checkError
succ "Build DONE"

popd &> /dev/null

###################
### DFU
###################

# goto bluenet scripts dir
pushd $BLUENET_DIR/scripts &> /dev/null

./dfuGenPkg.py -b "$BLUENET_BIN_DIR/bootloader_dfu.hex" -o "bootloader_"$version

checkError
succ "DFU DONE"

popd &> /dev/null

####################
### GIT release tag
####################

pushd $BLUENET_BOOTLOADER_DIR &> /dev/null

info "Create git tag for release"

# if old version existed
if [[ $version_str ]]; then
	echo $version > VERSION

	log "Updating changes overview"
	echo "Version $version:" > tmpfile
	git log --pretty=format:" - %s" "v$version_str"...HEAD >> tmpfile
	echo "" >> tmpfile
	echo "" >> tmpfile
	cat CHANGES >> tmpfile
	mv tmpfile CHANGES

	log "Add to git"
	git add CHANGES VERSION "include/version.h"
	git commit -m "Version bump to $version"

	log "Create tag"
	git tag -a -m "Tagging version $version" "v$version"

	# log "Push tag"
	# git push origin --tags
else
	# setup first time
	echo $version > VERSION

	log "Creating changes overview"
    git log --pretty=format:" - %s" >> CHANGES
    echo "" >> CHANGES
    echo "" >> CHANGES

	log "Add to git"
    git add VERSION CHANGES
    git commit -m "Added VERSION and CHANGES files, Version bump to $version"

	log "Create tag"
    git tag -a -m "Tagging version $version" "v$version"

	# log "Push tag"
    # git push origin --tags
fi

popd &> /dev/null