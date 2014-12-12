---------------------
AdrenoOpenCLLauncher
---------------------

This directory contains the "Adreno OpenCL SDK Launcher" application for Android.
This application provides a central graphical user interface for launching all
of the OpenCL sample programs contained in the SDK.

1. You must have the latest Android SDK and NDK installed onto your machine.

2. Make sure to have the latest ant installed (http://ant.apache.org/bindownload.cgi)

3. For Windows users, make sure you have Cygwin installed on your system

4. Ensure that your device is connected to your development PC (type 'adb devices' to check this).

5. Setup environment variables (for example, in .bashrc) for Anroid NDK and ant.  On
   Cygwin the following could be added to .bashrc (substituting for
   the Android SDK, NDK, and ant the paths where these were installed on your system):
   
   export ANDROID_SDK=/cygdrive/c/Android/android-sdk
   export ANDROID_NDK=/cygdrive/c/Android/android-ndk-r8b
   export PATH=$PATH:$ANDROID_SDK/tools:$ANDROID_SDK/platform-tools:$ANDROID_NDK:/cygdrive/c/Android/apache-ant-1.8.4/bin

6. To compile the "AdrenoOpenCLLauncher", run:
      
       'android.bat update project -p . -n AdrenoOpenCLLauncher -t android-14' 
          NOTE: On Linux or MAC OS X, the command will be 'android' instead of 'android.bat'            
       'ant debug'
       'adb install -r bin/AdrenoOpenCLLauncher-debug.apk'


7. To run the "AdrenoOpenCLLauncher", run the "Adreno OpenCL SDK Launcher" from the 
   Android launcher on the device.
