#!/bin/bash

#Defaults
relative_path_to_install="./bin"
mkdir -p $relative_path_to_install

function copy_package()
{
    if [ $# -lt 1 ]
    then
        echo "Usage: $FUNCNAME apk_name"
        exit
    fi

    apk_name=$1;shift;

    if [ -f bin/"$apk_full_name" ]; then
        if [ -f $relative_path_to_install ]; then
           cp bin/"$apk_full_name" $relative_path_to_install
        fi
    else
        echo "ERROR File not generated $apk_full_name"
        popd
        exit
    fi
    echo "============================================================"
    echo "Done Copying Package $apk_name"
    echo "============================================================"
    popd
}

all_samples=(`ls -d */ | grep -v Common | cut -d/ -f1`)
function usage() {
        cat <<-EOF
        usage: $(basename $0)  [-u|-h|-help]
EOF
}

while [ $# -gt 0 ] ; do
    case "$1" in
        -u|-h|-help)                usage;exit;;
    esac
    shift
done


targets_selected=( "${all_samples[@]}" )

for i in "${targets_selected[@]}"; do
     if [ -f "./""$i""/Android/bin/""$i""-debug.apk" ]; then
         echo copying "./""$i""/Android/bin/""$i""-debug.apk" to $relative_path_to_install
         cp "./""$i""/Android/bin/""$i""-debug.apk" $relative_path_to_install
     else
        echo "Not found""./""$i""/Android/bin/""$i""-debug.apk"
     fi
done
echo "Copying ./install.sh $relative_path_to_install"
cp ./Build/Android/install.sh $relative_path_to_install

if [ -z $OPENCL_TESTS ]; then
     echo "*** WARNING: set $OPENCL_TESTS environment variable pointing to /opencl_tests directory"
else
     cp ./bin/* $OPENCL_TESTS/bin/cl_examples/
fi

