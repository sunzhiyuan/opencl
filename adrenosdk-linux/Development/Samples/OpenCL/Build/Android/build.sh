#!/bin/bash
# Convenience script for building a Adreno SDK OpenCL Sample for Android


#Defaults
relative_path_to_install="../../apk/"
android_version="android-19"

function clean_sample() 
{
    if [ $# -lt 1 ]
    then
        echo "Usage: $FUNCNAME apk_name"
        exit
    fi

    apk_name=$1;shift;


    pushd "../../""$apk_name""/Android"
    echo "============================================================"
    echo "Cleaning Package $apk_name"
    echo "============================================================"

    $ANDROID_NDK/ndk-build clean
    ant debug clean
    popd
}

function rebuild_common()
{
    echo "============================================================"
    echo "Rebuilding Common"
    echo "============================================================"
    android_bat_found=$(which android.bat)
    if [ $? -eq 0 ]; then
        ANDROID_CMD="android.bat"
    else
        ANDROID_CMD="android" 
    fi

    pushd ../../Common
    if [ ! -f project.properties ]; then
        echo "android.library=true" >> project.properties    
    fi

    $ANDROID_CMD update project -p . -n Common -t $android_version
    
    ant debug clean
    ant debug

    echo "============================================================"
    echo "Done Rebuilding Common"
    echo "============================================================"
    popd
}

function rebuild_sample()
{
    if [ $# -lt 1 ]
    then
        echo "Usage: $FUNCNAME apk_name"
        exit
    fi

    apk_name=$1;shift;

    root=../../../../..

    if [ -e $root/Bin ]; then
        mkdir -p $root/Bin
    fi

    if [ ! -f $root/Bin/ResourcePacker ] || [ ! -f $root/Bin/ShaderPreprocessor ]; then
        echo "============================================================"
        echo "Copying assets"
        echo "============================================================"
	
        platform=$(uname -s)
        RESOURCE_PACKER=$root/Development/Tools/ResourcePacker/Build/Linux
        SHADER_PREPROCESSOR=$root/Development/Tools/ShaderPreprocessor/Build/Linux
        
        if [ $platform == 'Darwin' ]; then
            machine_type=$(uname -m)
            if [ $machine_type == 'x86_64' ]; then
                RESOURCE_PACKER=$root/Development/Tools/ResourcePacker/Build/OSX/x64
                SHADER_PREPROCESSOR=$root/Development/Tools/ShaderPreprocessor/Build/OSX/x64
            else
                RESOURCE_PACKER=$root/Development/Tools/ResourcePacker/Build/OSX/x86
                SHADER_PREPROCESSOR=$root/Development/Tools/ShaderPreprocessor/Build/OSX/x86
            fi
        fi

        make -C "$RESOURCE_PACKER"
        make -C "$SHADER_PREPROCESSOR"

		chmod 777 $root/Development/Samples/Build/linux/SampleShaderCompiler.sh
		chmod 777 $root/Development/Samples/Build/linux/SampleAssetCompiler.sh

        pushd $PWD
        cd $root/Development/Samples/Build/linux; 
        echo "============================================================"
        echo "Preparing shaders"
        echo "============================================================"
        ./SampleShaderCompiler.sh; 
        echo "============================================================"
        echo "Preparing assets... (might take a while if first time)"
        echo "============================================================"
        ./SampleAssetCompiler.sh
        popd

        cp -r -p -f $root/Development/Assets/Samples/Kernels $root/Bin/Samples/Kernels			
        cp -r -p -f $root/Development/Assets/Samples/Meshes $root/Bin/Samples/Meshes
        cp -r -p -f $root/Development/Assets/Samples/Textures/*.tga $root/Bin/Samples/Textures

    fi


    pushd "../../""$apk_name""/Android"
    echo "============================================================"
    echo "Rebuilding Package $apk_name"
    echo "============================================================"



    apk_full_name="$apk_name"-debug.apk
    lib_name=lib"$apk_name".so
    mkdir -p assets/Samples/Kernels
    if [ -f InstallAssets.sh ]; then
        # Re-copy kernels again, in case there is an update
        echo "Re-copy kernels"

        if [ ! -e "$root/Bin/Samples/Kernels" ]; then
            mkdir "$root/Bin/Samples/Kernels"
        fi

        cp -r -p -f $root/Development/Assets/Samples/Kernels/* $root/Bin/Samples/Kernels
        chmod 777 InstallAssets.sh
        ./InstallAssets.sh
    fi

    if [ -f bin/"$apk_full_name" ]; then
        echo "Deleting existing binary"
        rm bin/$apk_full_name
    fi
    
    android_bat_found=$(which android.bat)
    if [ $? -eq 0 ]; then
        ANDROID_CMD="android.bat"
    else
        ANDROID_CMD="android" 
    fi
    $ANDROID_CMD update project -p . -n $apk_name -t $android_version
    
	mkdir -p src    

    cd ./jni
    $ANDROID_NDK/ndk-build clean
    $ANDROID_NDK/ndk-build
    cd ..

    #check if the *.so is built
    lib_loc=$(find ./libs -name $lib_name)
    if [ ! -f "$lib_loc"  ]; then
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        echo "!!!Failed to build $lib_name!"
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" 
        popd
        exit
    fi

    ant debug clean
    ant debug

    if [ -f bin/"$apk_full_name" ]; then
        if [ ! -e "$relative_path_to_install" ]; then
            mkdir $relative_path_to_install
        fi
        cp -f bin/"$apk_full_name" "$relative_path_to_install"
    else
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        echo "!!!ERROR File not generated $apk_full_name"
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        popd
        exit
    fi
    echo "============================================================"
    echo "Done Rebuilding Package $apk_name"
    echo "============================================================"
    popd
}

all_samples=(`ls -d ../../*/ | cut -d/ -f3`)
targets_selected=()
all=0
install=0
clean=0
build=1
function usage() {
		cat <<-EOF
        usage: $(basename $0)  [-u|-h|-help] [-a] [-t SAMPLE] [-t SAMPLE] [-c] [-b] [-i] [-at ANDROID_TARGET]

        -t TargetSample           Android Device on which to install
        -a                        Select All
        -i                        Install on device
        -c                        Clean and exit
        -no_build                 Skip Build Step
        -at                       Android target id, i.e. android-19
EOF
}

while [ $# -gt 0 ] ; do
    case "$1" in
        -u|-h|-help)                usage; build=0 ;;
        -t)                         shift;targets_selected=( "${targets_selected[@]}" "$1" );;
        -a)                         all=1;;
        -i)                         install=1;;
        -c)                         clean=1;;
        -no_build)                  build=0;;
        -at)                        shift;android_version=$1;;
    esac
    shift
done


if [ ${#targets_selected[*]} -eq 0 ] ; then
    # Default to all if nothing selected
    all=1;
fi

if [ $all -eq 1 ];then
    targets_selected=( "${all_samples[@]}" )
fi

if [ $clean -eq 1 ];then
    for i in "${targets_selected[@]}"; do
        if [ -d "../../""$i""/Android" ]; then
            clean_sample $i
            rm -rf "../../""$i""/Android/assets"
            rm -rf "../../""$i""/Android/libs"
            rm -rf "../../""$i""/Android/obj"
         fi
    done
    echo "Done Clean"
    exit
fi


if [ $build -eq 1 ];then
    rebuild_common
    for i in "${targets_selected[@]}"; do
        if [ "$i" != "Build" ]; then
            if [ -d "../../""$i""/Android" ]; then
                echo "Build Sample $i"
                rebuild_sample $i
            fi
        fi
    done
fi

if [ $install -eq 1 ]; then
    echo "============================================================"
    echo "Installing "
    echo "============================================================"

    for i in "${targets_selected[@]}"; do
		if [ "$i" != "Build" ]; then
			if [ -d "../../""$i""/Android" ]; then
            ./install.sh -t $i
			fi
		fi
    done
fi
