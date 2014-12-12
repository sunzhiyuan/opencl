#!/bin/bash
export DYLD_LIBRARY_PATH=./lib/QtGui.framework/Versions/4:./lib/QtCore.framework/Versions/4:./:${DYLD_LIBRARY_PATH}
export DYLD_FRAMEWORK_PATH=./lib:${DYLD_FRAMEWORK_PATH}
	
cd "${0%/*}"
./Tool

