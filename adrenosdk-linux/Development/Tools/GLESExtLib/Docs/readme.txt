//=============================================================================
// Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
//=============================================================================


======== Overview ========

The Qualcomm OpenGL ES 3.0 Extension Library allows Android developers to make use of OpenGL ES 3.0 functions and Qualcomm OpenGL ES extensions, which are not easily accessible on Android. The library does this by using the eglGetProcAddress function to obtain OpenGL ES 3.0 functions and Qualcomm OpenGL ES Extensions for use in your application. The library includes the OpenGL ES, EGL, and KHR header files required by the library to load OpenGL ES 3.0 and Qualcomm OpenGL ES extensions.


======== Notes ========

* Qualcomm OpenGL ES Extensions can only be used with Qualcomm GPU's (e.g. Adreno's). Please keep in mind that different Qcomm-extensions are supported in different versions of Adreno.

* The OpenGL ES 3.0 binary file is "libGLESv2" not "libGLESv3", as per GLES3 implementation guidelines. Note that for OpenGL ES 2.0 the binary lib name is also: "libGLESv2".

	

======== Setup ========

To create an application using the OpenGL ES 3.0 Extension Library:

* Add the library source files glesextlib.cpp and glesextlib.h to your project. Both of these files are located in the GLESExtLib/Src folder and should be built with your project.
* Your application must reference the OpenGL ES, EGL, and KHR header files included with the OpenGL ES 3.0 Extension Library and located in the GLESExtLib/Inc folder.
  These files are used by the library to ensure the appropriate function definitions are included.
* GLESEXTLIB must be defined in your preprocessor definition. This preprocessor define is used in the OpenGL ES header files to define the OpenGL ES 3.0 functions.

Using the library is simple: create an instance of the CGLESExtLib class and call the Create() method.

	CGLESExtLib extLib;
	extLib.Create();

After the Create() method is called, all OpenGL ES 2.0, 3.0 and extensions will be available to use directly as functions. Function names and parameters are defined in glesextlib.h.

Additional notes:
     * Qualcomm OpenGL ES Extensions will only be available on Qualcomm GPU’s (e.g. Adreno)
     * Different versions of Adreno may support different Qualcomm OpenGL ES Extensions
     * The Create() method must be called after the libGLESv2 library is loaded and before any OpenGL ES 3.0 functions or Qualcomm Extensions are called
     * Any functions that are unsupported by the GPU will be printed to the log when the Create() method is called (the console in Windows, logcat in Android)
