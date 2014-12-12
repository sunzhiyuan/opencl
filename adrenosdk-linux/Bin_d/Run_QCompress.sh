#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then

	fileOutput=$(file "./QCompress.app/Contents/MacOS/Tool")
	if [[ "$fileOutput" =~ "i386" ]]; then
		requiredArch=x86
	else
		requiredArch=x64
	fi
	export DYLD_LIBRARY_PATH=../Development/External/Qt/OSX/lib/QtGui.framework/Versions/4:../Development/External/Qt/OSX/lib/QtCore.framework/Versions/4:../Development/Lib/osx/$requiredArch/:../Development/External/FreeImage/3.16/osx/$requiredArch/:${DYLD_LIBRARY_PATH}
	export DYLD_FRAMEWORK_PATH=../Development/External/Qt/OSX/lib:${DYLD_FRAMEWORK_PATH}
	
	./QCompress.app/Contents/MacOS/Tool

elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	
		fileOutput=$(file "./QCompress")
	if [[ "$fileOutput" =~ "32-bit" ]]; then
		requiredArch=x86
	else
		requiredArch=x64
	fi
	
	export LD_LIBRARY_PATH=./Development/External/FreeImage/3.16/ubuntu/$(requiredArch)/:${LD_LIBRARY_PATH}
	LD_PRELOAD="./libfreeimage.so" ./QCompress
fi

