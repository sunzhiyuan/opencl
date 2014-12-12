#!/bin/bash


# Couldn't find "goto" statement for bash (like the batch script equivalent of this file has)
# Need to find a way to report/handle build failure for "InstallAssets" and "ant debug" 

curDir=${PWD}	# Store the current working directory..


# If no sample name was specified as 1st param...
if [[ $1 = "" ]]
then
  # :errorNoSampleName
  echo "Error. No sample name defined or not executing from Sample root."
  echo "Usage: build_android SampleName"
  exit 1	# Return 1 as error-code for no name
fi




cd ./$1/Build/android/jni		# Move to "Android/jni" folder
source /etc/environment		# Bring in environment variables from "etc/environment" file
${ANDROID_NDK}/ndk-build clean	# Do an NDK-build clean of our project
${ANDROID_NDK}/ndk-build	# Do an NDK-build of our project

cd ..						# Move back to "Android" folder
./InstallAssets.sh				# Copy assets to Bin/Bin_d folder
ant clean
android update project -p . -t android-14	# Update project so that ANT can generate or find build.xml and do a build
ant debug					# Build (debug version of) project with ANT
ant installd					# Install binaries to device


cd ${curDir}
