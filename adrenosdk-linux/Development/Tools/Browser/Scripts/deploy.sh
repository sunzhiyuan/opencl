#!/bin/bash

scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo $scriptDir

if [ "$(uname)" == "Darwin" ]; then
	echo === OSX ===
	echo Copying the browser launcher.
    
    mkdir -p $scriptDir/../../../../Bin/

    cp -p -v $scriptDir/../../../Tools/Browser/Scripts/OSXBrowser.sh $scriptDir/../../../../Bin/
	
    # Set executable permissions
    chmod 777 $scriptDir/../../../../Bin/OSXBrowser.sh
	
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	echo === Linux ===
	echo Copying the browser launcher.
    
    mkdir -p $scriptDir/../../../../Bin/

    cp -v -p -u $scriptDir/../../../Tools/Browser/Scripts/LinuxBrowser.sh $scriptDir/../../../../Bin/

	# Set executable permissions
	chmod 777 $scriptDir/../../../../Bin/LinuxBrowser.sh
fi
