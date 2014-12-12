-----------------------------------------
How to build samples for Android platform
2011 Qualcomm - Advanced Content Group
-----------------------------------------

The following steps are required to build this native sample for an Android platform:

1. You must have the latest Android SDK and NDK installed onto your machine.

2. Make sure to have the latest Eclipse build installed.

3. Make sure you have Cygwin installed on your system.

4. Ensure that your device is connected to your development PC (type 'adb devices' to check this).

5. Ensure that you can compile the native applications within the NDK and launch on your 
   Android device via Eclipse (for more information about setting up your Android development
   platform, visit http://developer.android.com/index.html)

6. To compile this sample natively, we use the ndk-build system via Cygwin. Navigate to the
   Android/jni directory of this sample within Cygwin and run the following command:
   
   '$ANDROID_NDK/ndk-build'
   
   Where ANDROID_NDK is an environment variable pointing to the root location of the NDK
   
7. Open Eclipse and add this newly compiled sample. Make sure to choose the latest API level
   that is installed on your machine.
   
8. Run the InstallMedia.bat file located in the main Android directory of the sample. This
   will copy over the data required to run the sample into the 'assets' directory.
   
9. Clean the project (Project->Clean...) to ensure everything is ready when APK is generated.

10. Run the sample (Run As->Android Application) 
