#!/bin/bash
# Copyright 2012, QuIC, Inc.

# Defaults:
device_type="GPU"
device_serial=""

usage() { # error_message
    cat <<-EOF
                usage: $(basename $0)
                -cpu               Test on CPU device
                -gpu               Test on GPU device
                -s                 Device Serial
EOF
}

while [ $# -gt 0 ] ; do
    case "$1" in
        -u|-h|-help)                usage ;;
        -cpu)                       device_type="cpu" ;;
        -gpu)                       device_type="gpu" ;;
        -s)                         shift; device_serial="$1" ;;
    esac
    shift
done

if [ -z $device_serial ]
then
    device_serial_cmd=""
else
    device_serial_cmd="-s $device_serial"
    echo "Running test on device $device_serial"
fi

adb $device_serial_cmd shell "cd /data/local/opencl/cl_demo_tests/wavelet_transform/ &&  ./cl_demo_test_waveletransform  test.ycbcr test_out.ycbcr 768 512 4 $device_type"
