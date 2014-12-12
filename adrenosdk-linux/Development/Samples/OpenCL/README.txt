Qualcomm Adreno SDK - OpenCL Samples
--------------------------------------

The OpenCL samples build system is developed for a Linux or OS X environment, but it is
possible to build them on Windows with cygwin or on the Windows command line.

Prerequisites:
* Latest Android SDK and Android NDK
* Apache Ant (http://ant.apache.org/bindownload.cgi)
* An Android device connected to your machine (type 'adb devices' to check this)
* For Windows users, Cygwin is required if you wish to use the included build shell script


Instructions
------------

1. Setup environment variables (for example, in .bashrc) for Anroid NDK and ant.  On
   Cygwin the following could be added to .bashrc (substituting for
   the Android SDK, NDK, and ant the paths where these were installed on your system):

   export ANDROID_SDK=/cygdrive/c/Android/android-sdk
   export ANDROID_NDK=/cygdrive/c/Android/android-ndk-r8b
   export PATH=$PATH:$ANDROID_SDK/tools:$ANDROID_SDK/platform-tools:$ANDROID_NDK:/cygdrive/c/Android/apache-ant-1.8.4/bin

   If using Windows but not cygwin, then make sure these paths are included in your
   system environment variables.


2. Samples can be built with the included shell scripts or manually via the command line. The shell scripts require you to use Linux or Cygwin if on a Windows machine.

   a. Automatic with Build Scripts (Linux/MacOS X/Cygwin)

      Before building, find out the android target ID by running "android list targets".
      The default target ID is android-19. Use option "-at ANDROID_ID" for the script below.

      i. Navigate into the OpenCL/Build/Android directory. ('cd Build/Android')
      ii. a. To build only: './build.sh -t <SampleName>'
          b. To build and install: './build.sh -i -t <SampleName>'
          c. To build all, './build.sh'
          d. For options list, './build.sh -h'
      iii. To install to device: './install.sh -t <SampleName>'

      Note. In the root folder of OpenCL, "build_android.sh" can also be used. It directs to /Build/Android/build.sh for convenience.


      For Windows command line:

      i. Navigate to the OpenCL sample directory
      ii. To build all samples, run "install_android all". To upload to device, run "install_android apk".
	  iii. To build and upload individual test, run install_android Common" then "install_android APP_NAME"
	  iv. Run the sample on your device.

   b. Manual on Command Line

     Samples may also be built manually on the command line. This will work in Linux, Mac OS X, Windows with Cygwin, or Windows command line.

     i. Before building any sample, you must first build the 'Common' library that is used by
        all of the samples.  This only needs to be built once and is done by executing the following
        commands:

       'cd Common'
       'android.bat update project -p . -n Common -t android-14'
       'ant debug'

       ***NOTE: On Linux or MAC OS X, the command will be 'android' instead of 'android.bat'

	 ii. The SDK installer may not create an Android/src folder properly. Make sure the sample has
		a "src" directory in the Android directory. If it doesn't then create one:

		'mkdir SampleName/Android/src'

     iii. To compile a sample natively, we use the ndk-build system.
         Navigate to the Android/jni directory of the sample run the following command:

        '$ANDROID_NDK/ndk-build'

        Where ANDROID_NDK is an environment variable pointing to the root location of the NDK

     iv. Then go to the Android/ directory for the sample (up one from jni/) and type:

       (where <SampleName> = the name of the sample, for example BandwidthTest if building the
                             BandwidthTest sample)

       'android.bat update project -p . -n <SampleName> -t android-14'
       './InstallAssets.sh' (Linux/OSX/Cygwin) or 'InstallAssets.bat' (Windows)
       'ant debug'
       'adb install -r bin/<SampleName>-debug.apk'

       ***NOTE: On Linux or MAC OS X, the command will be 'android' instead of 'android.bat'

       The following command-line options are supported by the samples:
         -e DEVICE [gpu|cpu|all]
         -e RUNTESTS [true|false]

3. A sample can also be run either automatically or manually.

   Automatically (Linux/Mac OS X/Cygwin only):
     './run_tests -t <SampleName>'

   Manually (any platform):
     'adb logcat -c'
     'adb shell am start -n com.qualcomm.<SampleName>/android.app.NativeActivity -e DEVICE gpu -e RUNTESTS true'
     'adb logcat | grep OpenCL'

4. Stop the sample from the UI or run:
   'adb shell am force-stop com.qualcomm.<SampleName>'


CL/GL interop
---------------------
In sample app PostProcessCLGLES, it may be compiled to not share memory between CL and GL.
This is to demonstrate the advantage of CL/GL interop. The default behavior is to share
memory between CL/GL. If memory is not shared, it needs to copy memory from GL to host,
perform CL operation, then copy memory back to GL. The result of this is more intermediate
steps and lower framerate.

To show the difference, here are the steps:
    *  Turn off default framerate limiting option by running command:
           adb shell setprop debug.egl.swapinterval 0
       This allows the app to render as fast as the GPU allows.

    To manually build the example:
       1. Navigate to Samples/OpenCL/PostProcessCLGLES/Android/jni and run "ndk-build clean"
       2. Compile with the command:
              ndk-build
       3. Go up one directory to the Android folder, 
           a. copy assets by running InstallAssets.sh or InstallAssets.bat
           b. run "ant debug" and "ant installd"
       4. Run the app on device and monitor the framerate

    To compile without buffer sharing:
       Use the same steps as above but with following compiler option
           ndk-build APP_CFLAGS+=-DNO_SHARED_BUFFER

    Without buffer sharing, fps is expected to be much lower.
