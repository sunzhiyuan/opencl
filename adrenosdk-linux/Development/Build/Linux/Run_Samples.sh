#!/bin/bash
fileOutput=$(file ./$1)
if [[ "$fileOutput" =~ "i386" ]]; then
	requiredArch=x86
	echo "ARCH is x86"
else
	requiredArch=x64
	echo "ARCH is x64"
fi

if [ "$(uname)" == "Darwin" ]; then
	export DYLD_LIBRARY_PATH=./:../Development/Lib/osx/$requiredArch${DYLD_LIBRARY_PATH}
	./$1
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	export LD_LIBRARY_PATH=./:${LD_LIBRARY_PATH}
	LD_PRELOAD="./libGLESv2.so ./libEGL.so" ./$1
fi
