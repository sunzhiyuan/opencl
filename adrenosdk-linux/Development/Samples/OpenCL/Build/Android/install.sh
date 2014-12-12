#!/bin/bash
# Copyright 2013, QuIC, Inc.


function local_adb_push(){
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

function local_android_discover_adb_devices(){
   if [ -z $ANDROID_SERIAL ];then
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
}

function install_apps() {
    device_serial_cmd=""
    while [ $# -gt 0 ] ; do
            case "$1" in
            -s)                shift; device_serial="$1";device_serial_cmd="-s $1";;
            -t)                shift; test="$1";;
        esac
        shift
    done

   echo "Installing $test"

   adb $device_serial_cmd install -r $test
}



#defaults
device_serial=""
device_serial_cmd=""
auto=0
test_list=()
app_install_target_base_dir="/data/local/opencl/cl_examples"

usage() {
        cat <<-EOF
        usage: $(basename $0) [-v|-verbose] [-s DEVICE_SERIAL]  [-u|-h|-help] [-a] [-t TEST -t TEST..]

        -s DEVICE_SERIAL          Android Device on which to install
        -a                        Automatically discover devices, supersceeds -s option
        -t                        Test to install
EOF
}

while [ $# -gt 0 ] ; do
    case "$1" in
        -u|-h|-help)                usage ;;
        -v|-verbose)                local verbose=1;local verbose_flag="-verbose";;
        -s)                         shift; device_serial="$1";device_serial_cmd="-s $1";;
        -a)                         auto=1;;
        -t)                         shift; selected_list=( "${selected_list[@]}" "$1" );;
    esac
    shift
done

if [ ${#selected_list[*]} -eq 0 ]; then
       test_list=(`find ../../ -type f -name "*-debug.apk"`)
else
    for sample in "${selected_list[@]}"; do
        test_list=( "${test_list[@]}" `find ../../ -type f -name "$sample-debug.apk"` )
    done
fi


if [ $auto -eq 0 ]; then
    for test in "${test_list[@]}"; do
        install_apps -t $test
    done
else
    local_android_discover_adb_devices
    for sample in "${test_list[@]}"; do
    	for dev in $ANDROID_DEVICE_LIST; do
            install_apps -s $dev  -t $sample
        done
    done
fi
