Qualcomm Snapdragon SDK for Android Sample Application

SmartShutter Version 1.0

== Purpose ==
This program suite illustrates the use of the APIs in the Snapdragon SDK for 
Android to integrate the facial processing feature into a camera application 
on supported devices running Android OS 4.0 and above.  
(Please see https://developer.qualcomm.com/mobile-development/add-advanced-features/snapdragon-sdk-android/snapdragon-sdk-android-device-compatibility for a list of compatible devices.)  
Studying these files should provide the information required for integrating an 
Android camera-based app with facial processing features to trigger the shutter 
when faces in the picture are smiling and facing the camera.

== Prerequisites ==
This code assumes knowledge of general Android development, familiarity with 
the Android NDK, and knowledge of embedded camera operation in an Android application.  
Additionally, you must first download and install the Snapdragon SDK for Android 
located at http://developer.qualcomm.com/snapdragonsdk


== Description ==
The SmartShutter app is an enhanced camera application with the 
option to use an automated shutter that waits for the primary faces in the photo 
to be smiling and facing the camera in order for the photo to be taken.

Download 
and compile the sample app in Eclipse or another Android development environment 
and run it on a compatible device (see Purpose section) to note the usage of the 
facial processing features in the application.

Instructions (assuming Eclipse development environment):


1. Create a new project in Eclipse from Existing Sample Code. 

2. Navigate to the unzipped download folder for the SmartShutter source.

3. Import the sd-sdk-facial-processing.jar file into your project.

4. Make sure the sd-sdk-facial-processing.jar file is included in both the Build 
Path and is selected in the Order and Export tab under Build Settings. 

5. Build and Run the app on a compatible device.

  

When launching the application for the first time, verify that the device is compatible 
with the facial processing features by seeing two face icons when opening the Settings 
menu in the bottom right of the screen.  If these icons do not exist, then run the test 
app from the Snapdragon SDK for Android to verify that the facial processing feature is 
supported.

Use the Settings icon to toggle on and off the facial processing features.  
The top icon with the star next to the face will activate the SmartShutter facial processing 
capabilities when the shutter is pressed.  The second icon with the eyes and mouth will 
toggle on and off a preview display of what the facial processing is analyzing in real-time 
with a box around each face.  Use the second icon to visually see the box around the face 
change color as you smile.  When the facial processing feature (top icon) is on, tapping 
the shutter button will cause the camera to start waiting for all the faces in the frame 
to be smiling and facing the camera with eyes open before the photo is automatically taken.  
You can then review the taken photo and choose whether to accept it into the gallery or 
cancel and discard the photo.


Example:
 
1. Make sure the camera is set to the front-facing camera so that you see yourself 
in the picture. 

2. Look at the screen without smiling.
 
3. Ensure the facial processing feature (top icon in Settings) is active. 

4. Tap the shutter button to initiate scanning of the camera frames.  Notice no 
photo is taken. 

5. Slowly smile until the shutter triggers and presents the taken photo. 

6. Accept the photo to save to your gallery, or discard the photo to delete it.

 

Note that the threshold for recognizing the smile can be adjusted at the application 
level as noted in SmartShutterActivity.java.  This applies as well for other thresholds 
such as measurement of eyes open / closed and gaze direction.


== Contents ==
The following 
code files are included:

SmartShutterActivity.java - the majority of the code logic 
is in this file, including all of the calls to the facial processing engine

DrawView. 
Java - provides overlay drawing routines for the facial detection and facial processing 
data of each face

CameraSurfacePreview.java - provides configuration of the preview 
surface for the camera frames

ImageConfirmation.java - this activity processes the image 
after it is taken by allowing the user to accept the image into the photo gallery or 
to discard it



For more information on Snapdragon SDK for Android, please 
visit http://developer.qualcomm.com/snapdragonsdk


Version History

1.0 - Initial release 02/27/2014
