#!/bin/bash
# Copyright 2012, QuIC, Inc.


#---------------------------------------------------------------------------
# Function android_setup_and_discovery
# Setup for multiple target support
#---------------------------------------------------------------------------
function android_setup_and_discovery()
{
    if [ -z $ANDROID_SERIAL ]
    then
	echo "Discovering Devices"
	ANDROID_DEVICE_LIST=(`adb devices 2>&1 | tail -n+2 | cut -f1`)
    for dev in ${ANDROID_DEVICE_LIST[@]}
	do
	    echo "Found device $dev "
	done
    else
	echo "Using device $ANDROID_SERIAL defined by ANDROID_SERIAL"
	ANDROID_DEVICE_LIST=$ANDROID_SERIAL

    fi


    export ANDROID_DEVICE_LIST

    for dev in ${ANDROID_DEVICE_LIST[@]}
    do
	echo "Performing Setup on device $dev"
	adb -s $dev wait-for-device
	adb -s $dev shell chmod 777 /data/busybox/busybox
	adb -s $dev shell /data/busybox/busybox --install
	adb -s $dev remount
	adb -s $dev wait-for-device
    done
}




function adb_push(){
    from_file=$2
    to_file=$3
    device_serial=$1
    if [ $device_serial = "default" ]
    then
        device_serial_cmd="";
    else
        device_serial_cmd="-s $device_serial"
    fi
    echo "Pushing device:$device_serial $2 to $3"
    `adb $device_serial_cmd push $2 $3`
    `adb $device_serial_cmd shell sync`

}

function adreno_sdk_install_device()
{
    if [ $# -lt 1 ]; then
        device_serial_cmd= ""
        device_serial="default"
    else
        device_serial=$1
        device_serial_cmd="-s $1"
    fi

    echo "==========================================="
    echo "=  Installing Adreno OpenCL SDK binaries on device $device_serial"
    echo "==========================================="


    echo "Setting Root on device $1"
    adb $device_serial_cmd root
    echo "Setting Remount on device $1"
    adb $device_serial_cmd remount

    adb $device_serial_cmd shell mkdir -p /data/local/opencl_sdk

    pushd bin
    file_list=(`find . -type f `) 
  
    for FILE in ${file_list[@]}; do
        if [[  $FILE =~ ".apk" ]]; then
            echo "Installing APK $FILE"
            adb -s $device_serial install -r $FILE
        elif [[ $FILE =~ "*.sh" ]]; then 
            echo "SKIP $FILE"
        else
            local dir=`dirname $FILE`
            local dir=`echo "${dir#./}"`
            adb shell "mkdir -p /data/local/opencl_sdk/$dir"
            adb_push $device_serial $FILE /data/local/opencl_sdk/$dir
        fi

    done
    popd

}

function adreno_sdk_install()
{
    echo "Installing binary components of the Andreno OpenCL SDK on device"
    android_setup_and_discovery

    for dev in ${ANDROID_DEVICE_LIST[@]}
    do  
        adreno_sdk_install_device $dev
    done
}

adreno_sdk_install


