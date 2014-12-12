Adreno SDK Linux Build Instructions
Copyright (c) 2013 Qualcomm Technologies, Inc.

IMPORTANT PREREQUISITE

FreeImage must be installed on your Linux machine in order to build with the Adreno SDK. We do not distribute the Linux FreeImage libraries with the Adreno SDK. You can install the FreeImage library on Ubuntu using:

'sudo apt-get install libfreeimage-dev'


BUILDING THE ADRENO SDK

Each sample or Tool has its own Linux makefile, but a master makefile has been created in the same directory as this README. To build the entire SDK, navigate to the SDK/Development/Build directory in a terminal window and use 'make' to initiate a Linux build.