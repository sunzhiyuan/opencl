#!/bin/bash
#clear


#Defaults
device_type="gpu"
timeout=120
test_to_run=""
device_serial_cmd=""
test_set=()
run_log="sample_results.log"

if [ -n $CUR_OPENCL_TEST_ROOT ]; then
    logpath="$CUR_OPENCL_TEST_ROOT""/logs/cl_examples"
else
    logpath="./logs"
fi

test_list=(\
BandwidthTest \
MatrixMatrixMul \
ImageBoxFilter \
Concurrency \
ImageMedianFilter \
MatrixTranspose \
VectorAdd \
DeviceQuery \
ImageRecursiveGaussianFilter \
MatrixVectorMul \
DotProduct \
ImageSobelFilter \
FFT1D \
FFT2D \
ParallelPrefixSum \
)

#trap destructor SIGINT


function destructor(){
	echo "Terminating"
        adb shell am force-stop com.qualcomm.$test_name
        if [ -n $subshell_pid ]; then
            kill -9 $subshell_pid >> /dev/null
        fi
}

function usage() {
    cat <<-EOF
                usage: $(basename $0) [-cpu] [-gpu] [-t TEST1] [-t TEST2] [-s DEVICE_SERIAL] [-l LOGPATH]

                -cpu|cpu                   Use CPU device only
                -gpu|gpu                   Use GPU device only
                -t|-test                   Test to run, default $test_to_run
                -d|-device_type            Device type cpu|gpu, default $device_type
                -s|-serial                 Device serial, default=first device found by adb
                -l|-logpath                Logpath, default $logpath

                Test names:
EOF
    for i in "${test_list[@]}"; do
    echo -e "\t\t\t$i"
    done
exit 0;
}

while [ $# -gt 0 ] ; do
    case "$1" in
        -u|-h|-help)                usage ;;
        -cpu|cpu)                   device_type="cpu" ;;
        -gpu|gpu)                   device_type="gpu" ;;
        -t|-test)                   shift; test_set=( "${test_set[@]}" "$1" );;
        -d|-device_type)            shift; device_type="$1";;
        -s|-serial)                 shift; device_serial="$1";device_serial_cmd="-s $1";;
        -l|-logpath)                shift; logpath="$1";;
        -timeout)                   shift; timeout="$1";;
    esac
    shift
done


if [ ${#test_set[@]} -eq 0 ]; then
    test_set=("${test_list[@]}")
fi

log_path="./logs"
mkdir -p $log_path
interval=0.5

if [ -f $run_log ]; then
    rm -f $run_log
fi


function force_exit_timeout(){
        echo "$test_name Result: FAILED - TIMEOUT" | tee -a $run_log
        echo "FAILED - TIMEOUT" | tee -a $log_file
        echo "Process $1 Exiting for $test_name"
        adb shell am force-stop com.qualcomm.$test_name
    }

for test_name in "${test_set[@]}"
do

    log_file="$logpath""/"$test_name".log"
    mkdir -p $logpath
    if [ -f $log_file ]; then
        echo "Deleting existing logfile"
        rm -f $log_file
    fi
    touch $log_file
    echo "================================================================="
    echo " Running tests: $test_name "
    echo " Log: $log_file"
    echo " Timeout:  $timeout sec     "
    echo "================================================================="
    sleep .5
    adb $device_serial_cmd logcat -c

    cmd="adb shell $device_serial_cmd am start -S -n com.qualcomm.""$test_name""/com.qualcomm.common.AdrenoNativeActivity -e DEVICE $device_type -e RUNTESTS true"
    echo "Issuing command to start app"
    echo "$cmd"
    $cmd

    # Wait until we find a reason to exit in the monitoring loop process.
    # The process just exits when the condition is met
    start_time="$(date +%s)"
    while [[ "1" == "1" ]]; do
            sleep $interval
            end_time="$(date +%s)"
            # Check test results
            logcat_line=$( adb $device_serial_cmd logcat -d | grep -i opencl )
            if [[ "$logcat_line" =~ "PASSED" ]] || [[ "$logcat_line" =~ "FAILED" ]] || [[ "$logcat_line" =~ "ERROR" ]]; then
                if [[ "$logcat_line" =~ "PASSED" ]]; then
                    echo -e "\n$test_name Result: PASSED" | tee -a $run_log
                elif [[ "$logcat_line" =~ "FAILED" ]] || [[ "$logcat_line" =~ "ERROR" ]]; then
                    echo -e "\n$test_name Result: FAILED" | tee -a $run_log
                    adb $device_serial_cmd logcat -d >> $log_file
                 fi
                adb $device_serial_cmd logcat -d | grep -i opencl >> $log_file
                break
            fi
            elapsed=$(expr $end_time - $start_time)

            #If test did not pass or Fail, check timeout
            if [ $elapsed -ge $timeout ]; then
                echo "ERROR Timeout"
		# Add Full logcat in case of timeout
                adb $device_serial_cmd logcat -d  >> $log_file
                force_exit_timeout
                break;
            fi
    	echo -n "."
    done
    adb shell am force-stop com.qualcomm.$test_name
    echo "log file is $log_file"
    cat $log_file
done


