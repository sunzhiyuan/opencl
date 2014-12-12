#!/bin/bash
fileOutput=$(file "./ResourcePacker")
echo "=== Run_ResourcePacker === $fileOutput"
if [[ "$fileOutput" =~ "i386" ]]; then
	echo "Running ResourcePacker x86" 
	requiredArch=x86
else
	echo "Running ResourcePacker x64"
	requiredArch=x64
fi

if [ "$(uname)" == "Darwin" ]; then
	pwd
	export DYLD_LIBRARY_PATH=./:../Development/Lib/osx/$requiredArch:../Development/External/FreeImage/3.16/osx/$requiredArch:${DYLD_LIBRARY_PATH}
	./ResourcePacker  $1 $2
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	export LD_LIBRARY_PATH=./:${LD_LIBRARY_PATH}
	LD_PRELOAD="./libfreeimage.so" ./ResourcePacker  $1 $2
fi