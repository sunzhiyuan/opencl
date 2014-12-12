/* ======================================================================
 *  Copyright � 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 *  QTI Proprietary and Confidential.
 *  =====================================================================
 *  
 * @file:   SmartShutterActivity.java
 *
 */
package com.qualcomm.sdk.smartshutterapp;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import com.qualcomm.snapdragon.sdk.face.FaceData;
import com.qualcomm.snapdragon.sdk.face.FacialProcessing;
import com.qualcomm.snapdragon.sdk.face.FacialProcessing.PREVIEW_ROTATION_ANGLE;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.AnimationDrawable;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.ShutterCallback;
import android.hardware.Camera.Size;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.CheckBox;
import android.widget.FrameLayout;
import android.widget.ImageView;


public class SmartShutterActivity extends Activity implements Camera.PreviewCallback{
	Camera cameraObj;				// Accessing the Android native Camera.
	FrameLayout preview;
	CameraSurfacePreview mPreview;
	FacialProcessing faceProc;
	DrawView drawView;
	FaceData[] faceArray;			// Array in which all the face data values will be returned for each face detected. 
	private ImageView cameraButton;
	private ImageView settingsButton;
	private ImageView switchCameraButton;
	private ImageView menu;
	private ImageView faceDetectionButton;
	private ImageView perfectPhotoButton;
	private ImageView galleryButton;
	private ImageView flashButton;
	Animation animationFadeOut;
	Display display;
	AnimationDrawable frameAnimation;
	CheckBox smile;
	CheckBox gazeAngle;
	CheckBox eyeBlink;	

	private int FRONT_CAMERA_INDEX= 1;
	private int BACK_CAMERA_INDEX = 0;
	private boolean _qcSDKEnabled;
	private static boolean switchCamera = false;
	private static boolean settingsButtonPress;
	private static boolean faceDetectionButtonPress;
	private static boolean perfectModeButtonPress;
	private static boolean cameraButtonPress;
	private static boolean animationPress;
	private static String flashButtonPress;
	private int displayAngle;	
	private boolean smileFlag;
	private boolean blinkFlag;
	private boolean horizontalGazeAngleFlag;
	private boolean verticalGazeAngleFlag;
	private static boolean activityStartedOnce;
	private int numFaces;
	private static String pathName;
	

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
		setContentView(R.layout.activity_main);
		
		initializeFlags();
		
		initializeCheckBoxes();		
		
		animationFadeOut = AnimationUtils.loadAnimation(SmartShutterActivity.this, R.anim.fadeout);
		
		initializeImageButtons();
		
		settingsButtonPress = false;	// We make the settings button press false every time the activity restarts. This helps in keeping track of the button press boolean. 		
		
		switchCameraActionListener();
		galleryActionListener();
		cameraActionListener();
		settingsActionListener();
		faceDetectionActionListener();	 
		perfectPhotoActionListener();
		flashActionListener();
		
		// Start default with Front Camera and initialize the necessary facial processing objects.
		startCameraAndInitialize();
		
		display = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();	
	}	
	
	private void initializeFlags() {
		_qcSDKEnabled = false;
		settingsButtonPress = false;
		faceDetectionButtonPress = false;
		perfectModeButtonPress =false;
		cameraButtonPress = false;
		animationPress = false;
		flashButtonPress = "FLASH_MODE_OFF";
		smileFlag=true;
		blinkFlag=true;
		horizontalGazeAngleFlag = true;
		verticalGazeAngleFlag = true;
		activityStartedOnce = false;
	}

	protected void onPause() {
        super.onPause();
        stopCamera();
    }

	protected void onDestroy(){
		super.onDestroy();
	}

	 protected void onResume() {
	        super.onResume();
	        if(cameraObj!=null)
	        {
	        	stopCamera();	        	
	        }	        
	        startCameraAndInitialize();
	 } 
		
	 // Stop the camera preview. release the camera. Release the Facial Processing object. Make the objects null. 
	 private void stopCamera() {
		
		if(cameraObj!=null)
		{
			cameraObj.stopPreview();
            cameraObj.setPreviewCallback(null);
            preview.removeView(mPreview);	            
            cameraObj.release();
            if(_qcSDKEnabled)
            {
	            faceProc.release();
	    		faceProc = null;
            }
		}	
		 cameraObj = null;
	}
	
	 // Start with the camera preview. Open the Camera. See if the feature is supported. Initialize the facial processing instance. 
	 private void startCameraAndInitialize() {
		
		// Check to see if the FacialProc feature is supported in the device or no. 
	     _qcSDKEnabled= FacialProcessing.isFeatureSupported(FacialProcessing.FEATURE_LIST.FEATURE_FACIAL_PROCESSING);
	     
		if(_qcSDKEnabled && faceProc == null)
		{			
			Log.e("TAG", "Feature is supported");				 
			faceProc= FacialProcessing.getInstance();				// Calling the Facial Processing Constructor. 				
		}
		else if(!_qcSDKEnabled && !activityStartedOnce)
		{
			Log.e("TAG", "Feature is NOT supported");
			AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(SmartShutterActivity.this);
	 
				// set title
				alertDialogBuilder.setTitle("Not Supported");
	 
				// set dialog message
				alertDialogBuilder
					.setMessage("Your device does not support Qualcomm's Facial Processing Feature. Continue with the normal camera.")
					.setCancelable(false)
					.setPositiveButton("OK",new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog,int id) {
								
						}
					  });
	
				// create alert dialog
				AlertDialog alertDialog = alertDialogBuilder.create();
 
				// show it
				alertDialog.show();
				activityStartedOnce=true;
		}
		
		if(!switchCamera)
		{
			cameraObj = Camera.open(FRONT_CAMERA_INDEX); 					// Open the Front camera  	
		}
		else
		{
			cameraObj = Camera.open(BACK_CAMERA_INDEX); 					// Open the back camera   	
		}	       		        	
		
		mPreview = new CameraSurfacePreview(SmartShutterActivity.this, cameraObj);	// Create a new surface on which Camera will be displayed. 
	    preview = (FrameLayout) findViewById(R.id.cameraPreview);
	    preview.addView(mPreview);
	    cameraObj.setPreviewCallback(SmartShutterActivity.this); 	

	}
	/*
	 * Function to Initialize all the image buttons that are there in the view and sets its visibility and image resources here. 
	 */
	private void initializeImageButtons() {
		cameraButton = (ImageView) findViewById(R.id.cameraButton);			// Camera Shutter Button
		galleryButton = (ImageView) findViewById(R.id.gallery);				
		galleryButton.setImageResource(R.drawable.gallery);		
		settingsButton = (ImageView) findViewById(R.id.settings);
		switchCameraButton = (ImageView) findViewById(R.id.switchCamera);
		switchCameraButton.setImageResource(R.drawable.switch_camera_front);
		switchCameraButton.setVisibility(View.INVISIBLE);					// Initially make switchCamera invisible. Make it visible only when the settings button is pressed. 
		menu = (ImageView) findViewById(R.id.menu);
		menu.setVisibility(View.INVISIBLE);					// Initially make menu invisible. Make it visible only when the settings button is pressed.
		perfectPhotoButton = (ImageView) findViewById(R.id.perfectMode);
		perfectPhotoButton.setVisibility(View.INVISIBLE);					// Initially make perfectMode invisible. Make it visible only when the settings button is pressed.	
		perfectPhotoButton.setImageResource(R.drawable.perfect_mode_off);
		flashButton = (ImageView) findViewById(R.id.flash);
		flashButton.setVisibility(View.INVISIBLE);							// Initially make flashButton invisible. Make it visible only when the settings button is pressed.	
		
		if(flashButtonPress == "FLASH_MODE_OFF")							// Change the flash image depending on the button that is being pressed. 
		{	
			flashButton.setImageResource(R.drawable.flash_off);
		}
		else
		{
			flashButton.setImageResource(R.drawable.flash_on);
		}
		
		// Draw Button facilitates the user to draw eyes and mouth on the camera preview. User can enable/disable. 
		if(!faceDetectionButtonPress)
		{
			faceDetectionButton = (ImageView) findViewById(R.id.faceDetection);
			faceDetectionButton.setImageResource(R.drawable.face_detection);
		}
		else
		{
			faceDetectionButton = (ImageView) findViewById(R.id.faceDetection);		
			faceDetectionButton.setImageResource(R.drawable.face_detection_on);
		}		
		faceDetectionButton.setVisibility(View.INVISIBLE);	
	}

	
	/*
	 * Initialize the Check Box Buttons. Initially it will be invisible. Will be visible only when the photo is to be taken. 
	 */
	private void initializeCheckBoxes() {
		smile = (CheckBox) findViewById(R.id.smileCheckBox);
		smile.setVisibility(View.GONE);
		smile.setTextColor(Color.YELLOW);
		gazeAngle = (CheckBox) findViewById(R.id.gazeAngleCheckBox);
		gazeAngle.setVisibility(View.GONE);
		gazeAngle.setTextColor(Color.YELLOW);
		eyeBlink = (CheckBox) findViewById(R.id.blinkCheckBox);
		eyeBlink.setVisibility(View.GONE);
		eyeBlink.setTextColor(Color.YELLOW);		
	}
	
	/*
	 * Function to detect the on click listener for the switch camera button. 
	 */
	private void switchCameraActionListener() {
		switchCameraButton.setOnClickListener(new OnClickListener() {
				
		@Override
		public void onClick(View arg0) {
			
				if(!switchCamera)		// Flag to check if the camera is switched to front or back. 
				{
					switchCameraButton.setImageResource(R.drawable.switch_camera_back);
					stopCamera();
					switchCamera = true;
					settingsButtonPress = false;
					startCameraAndInitialize();
					fadeOutAnimation();
				}
				else
				{
					switchCameraButton.setImageResource(R.drawable.switch_camera_front);
					stopCamera();
					switchCamera = false;
					settingsButtonPress = false;
					startCameraAndInitialize();
					fadeOutAnimation();
				}
			}
		});		
	}
	
	/*
	 * Function to detect the on click listener for the GALLERY button. 
	 */
	private void galleryActionListener() {
		galleryButton.setOnClickListener(new OnClickListener() {
				
		@Override
		public void onClick(View arg0) {
				Intent intent = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                intent.setType("image/*");
                intent.setAction(Intent.ACTION_GET_CONTENT);
                startActivityForResult(Intent.createChooser(intent, "Select Picture"), 0); 
			}
		});
	}
	
	/*
	 * Function to detect the on click listener for the camera shutter button.  
	 */
	private void cameraActionListener() {
		cameraButton.setOnClickListener(new OnClickListener() {
				
		@Override
		public void onClick(View arg0) {
			
				if(!perfectModeButtonPress)
				{
					cameraObj.autoFocus(new Camera.AutoFocusCallback() {
			            @Override
			            public void onAutoFocus(boolean success, Camera camera) {
			            	
							cameraObj.takePicture(shutterCallback, rawCallback, jpegCallback);
								
			            }
			        }); 
					
				}		
				else
				{			
					 cameraButton.setBackgroundResource(R.drawable.spin_animation);			// Get the background, which has been compiled to an AnimationDrawable object.
					 frameAnimation = (AnimationDrawable) cameraButton.getBackground();
					 
					 checkBoxVisiblity(true);		// As soon as the shutter button is pressed, make the check boxes visible. 			 
					 
					 // Start the animation (looped playback by default).
					 if(!animationPress)
					 {
						 frameAnimation.start();
						 animationPress=true;
						 cameraButtonPress = true;
					 }
					 else
					 {
						 checkBoxVisiblity(false);	// If the shutter button is stopped then make the check boxes invisible 
						 textBoxChecked(false);		// and un-check them.				 
						 frameAnimation.stop();
						 cameraButton.setBackgroundResource(R.drawable.shutter_button);
						 animationPress=false;		
						 cameraButtonPress = false;
					 }
				}
			}		
		});			
	}
	
	/*
	 * Function to detect the on click listener for the switch camera button. 
	 */
	private void settingsActionListener() {
		settingsButton.setOnClickListener(new OnClickListener() {
				
		@Override
		public void onClick(View arg0) {
				if(!settingsButtonPress)
				{
					if(_qcSDKEnabled)		// Disable the buttons if the facial processing feature is not supported. 
					{
						faceDetectionButton.setVisibility(View.VISIBLE);
						perfectPhotoButton.setVisibility(View.VISIBLE);
					}
					menu.setVisibility(View.VISIBLE);
					switchCameraButton.setVisibility(View.VISIBLE);
					if(switchCamera)// If facing back camera then only make it visible or else dont. 
						flashButton.setVisibility(View.VISIBLE);					
					settingsButtonPress=true;
				}
				else
				{
					faceDetectionButton.setVisibility(View.INVISIBLE);
					menu.setVisibility(View.INVISIBLE);
					switchCameraButton.setVisibility(View.INVISIBLE);
					perfectPhotoButton.setVisibility(View.INVISIBLE);
					flashButton.setVisibility(View.INVISIBLE);
					settingsButtonPress=false;
				}
			}
				
		});
	
		// On touch listener for the settings button to make it highlighted when pressed 
		settingsButton.setOnTouchListener(new View.OnTouchListener() {

			@Override
			public boolean onTouch(View arg0, MotionEvent arg1) {
				
				if (arg1.getAction()==MotionEvent.ACTION_DOWN)
				{
					settingsButton.setImageResource(R.drawable.settings_clicked);
				}
				else if(arg1.getAction()==MotionEvent.ACTION_UP)
				{
					settingsButton.setImageResource(R.drawable.settings);
				}
				return false;
			}
	    });
		
	}
	
	/*
	 * Function to detect the on click listener for the face_detection_on button. When enabled, it will draw the eyes and moth co-ordinates on the face. 
	 */
	private void faceDetectionActionListener() {
		faceDetectionButton.setOnClickListener(new OnClickListener() {
				
		@Override
		public void onClick(View arg0) {
				if(!faceDetectionButtonPress)
				{
					faceDetectionButton.setImageResource(R.drawable.face_detection_on);
					fadeOutAnimation();
					faceDetectionButtonPress=true;
					settingsButtonPress = false;
				}
				else
				{
					faceDetectionButton.setImageResource(R.drawable.face_detection);
					fadeOutAnimation();					
					faceDetectionButtonPress=false;
					settingsButtonPress = false;					
				}				
			}			
		});
		
	}
	
	
	/*
	 * Function to detect the action listener of the flash button. Will change the flash mode depending on the button click. 
	 */
	private void flashActionListener() {
		flashButton.setOnClickListener(new OnClickListener() {
				
		@Override
		public void onClick(View arg0) {
			Camera.Parameters params = cameraObj.getParameters();
			String flashMode = params.getFlashMode();
				if (flashMode == null) 
					return;
				else
				{					
					if(flashButtonPress =="FLASH_MODE_OFF")	// If already OFF then, make it ON and change the icon to ON
					{
						params.setFlashMode(Camera.Parameters.FLASH_MODE_ON);
						flashButton.setImageResource(R.drawable.flash_on);
						cameraObj.setParameters(params);
						fadeOutAnimation();
						flashButtonPress = "FLASH_MODE_ON";
						settingsButtonPress=false;
						return;
					}
					else	// If already ON then, make it OFF and change the icon to OFF
					{
						params.setFlashMode(Camera.Parameters.FLASH_MODE_OFF);
						flashButton.setImageResource(R.drawable.flash_off);
						cameraObj.setParameters(params);
						fadeOutAnimation();
						flashButtonPress = "FLASH_MODE_OFF";
						settingsButtonPress=false;
						return;
					}
				}	
			}		
		});
		
	}
	
	/*
	 * Function to detect the on click listener for the perfect photo mode button. 
	 */
	private void perfectPhotoActionListener() {
		perfectPhotoButton.setOnClickListener(new OnClickListener() {
				
		@Override
		public void onClick(View arg0) {
				if(perfectModeButtonPress)
				{
					perfectPhotoButton.setImageResource(R.drawable.perfect_mode_off);
					fadeOutAnimation();
					settingsButtonPress = false;
					perfectModeButtonPress = false;
				}
				else
				{
					perfectPhotoButton.setImageResource(R.drawable.perfect_mode_on);
					fadeOutAnimation();
					settingsButtonPress = false;
					perfectModeButtonPress = true;				
				}				
			}			
		});		
	}
	
	
	/*
	 * Function to handle the fade out animation when any of the menu. 
	 * 
	 */
	private void fadeOutAnimation() {
		
		if(_qcSDKEnabled)						// Disable the buttons if the facial processing feature is not supported. 
		{
			faceDetectionButton.startAnimation(animationFadeOut);
			perfectPhotoButton.startAnimation(animationFadeOut);
		}
		menu.startAnimation(animationFadeOut);
		switchCameraButton.startAnimation(animationFadeOut);
		
		if(switchCamera)
		{
			flashButton.startAnimation(animationFadeOut);
		}
		faceDetectionButton.setVisibility(View.GONE);
		menu.setVisibility(View.GONE);
		switchCameraButton.setVisibility(View.GONE);	
		perfectPhotoButton.setVisibility(View.GONE);
		flashButton.setVisibility(View.GONE);
		
	}
	
	
	/*
	 * Function to write an image to the file system for future viewing. 
	 */
	public static boolean WritePictureToFile(Context context, Bitmap bitmap) {
		File pictureFile = getOutputMediaFile();
		if (pictureFile == null) {
			Log.e("TAG", "Error creating media file, check storage permissions ");
			return false;
		}

		try {
			FileOutputStream fos = new FileOutputStream(pictureFile);
			bitmap.compress(Bitmap.CompressFormat.PNG, 100, fos);
			fos.close();
			Log.e("TAG", "Wrote image to " + pictureFile);

            MediaScannerConnection.scanFile(context, new String[] { pictureFile.toString() }, null, new MediaScannerConnection.OnScanCompletedListener() {
                public void onScanCompleted(String path, Uri uri) {
                    Log.i("ExternalStorage", "Scanned " + path + ":");
                    Log.i("ExternalStorage", "-> uri=" + uri);
                }
            });
            pathName = pictureFile.toString();
            Log.e("TAG", "Path Name = "+pathName);
			return true;

		} catch (FileNotFoundException e) {
			Log.d("TAG", "File not found: " + e.getMessage());
		} catch (IOException e) {
			Log.d("TAG", "Error accessing file: " + e.getMessage());
		}
		return false;
	}
	
	/** Create a File for saving an image or video */
	@SuppressLint("SimpleDateFormat")
	private static File getOutputMediaFile() {
		// To be safe, you should check that the SDCard is mounted
		// using Environment.getExternalStorageState() before doing this.

		File mediaStorageDir = new File(
				Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM), "SmartShutter");

		// This location works best if you want the created images to be shared
		// between applications and persist after your app has been uninstalled.

		// Create the storage directory if it does not exist
		if (!mediaStorageDir.exists()) {
			Log.e("TAG",	"failed to find directory "	+ mediaStorageDir.getAbsolutePath());
			if (!mediaStorageDir.mkdirs()) {
				Log.e("TAG","failed to create directory "+ mediaStorageDir.getAbsolutePath());
				return null;
			}
		}

		// Create a media file name
		String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
		File mediaFile = new File(mediaStorageDir.getPath() + File.separator+ "IMG_" + timeStamp + ".jpg");
		return mediaFile;
	}
	
	ShutterCallback shutterCallback = new ShutterCallback() {
		public void onShutter() {
			Log.d("TAG", "onShutter'd");
		}
	};
	
	PictureCallback rawCallback = new PictureCallback() {
		public void onPictureTaken(byte[] data, Camera camera) {
			Log.d("TAG", "onPictureTaken - raw");
		}
	};
	
	PictureCallback jpegCallback = new PictureCallback() {
		public void onPictureTaken(byte[] data, Camera camera) {	
			savePicture(data);
		}				

	};
	
	/*
	 * Function to take the raw YUV byte array and do the necessary conversions to save it. 
	 */
	private void savePicture(byte[] data) {
		Intent intent = new Intent(this, ImageConfirmation.class);
		// This is when smart shutter feature is not ON. Take the photo generally. 
		if(data != null)
		{			
			intent.putExtra("com.qualcomm.sdk.smartshutterapp.ImageConfirmation", data);
		}
		intent.putExtra("com.qualcomm.sdk.smartshutterapp.ImageConfirmation.switchCamera", switchCamera);
		intent.putExtra("com.qualcomm.sdk.smartshutterapp.ImageConfirmation.orientation", displayAngle);
		startActivityForResult(intent, 1);		
	}

	
	private void setFlagsTrue() {
		smileFlag = true;
		blinkFlag = true;
		horizontalGazeAngleFlag = true;		
		verticalGazeAngleFlag = true;
	}
	
	/*
	 * A helper function to handle the visibility of the check boxes. 
	 */
	private void checkBoxVisiblity(boolean visible) {
		
		if(visible)
		{
			smile.setVisibility(View.VISIBLE);
			gazeAngle.setVisibility(View.VISIBLE);
			eyeBlink.setVisibility(View.VISIBLE);
		}
		else
		{
			smile.setVisibility(View.INVISIBLE);
			gazeAngle.setVisibility(View.INVISIBLE);
			eyeBlink.setVisibility(View.INVISIBLE);
		}
		
	}
	
	/*
	 *  A helper function to handle the CHECK-MARK of the Check-Text Boxes. 
	 */
	private void textBoxChecked(boolean check) {
		if(check)
		{
			smile.setChecked(true);		
			eyeBlink.setChecked(true);
			gazeAngle.setChecked(true);
		}
		else
		{
			smile.setChecked(false);		
			eyeBlink.setChecked(false);
			gazeAngle.setChecked(false);
		}
		
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {		
		
		setFlagsTrue();
		int dRotation = display.getRotation();
		PREVIEW_ROTATION_ANGLE angleEnum = PREVIEW_ROTATION_ANGLE.ROT_0;
        
		switch(dRotation){
		case 0:  // Device is not rotated
			displayAngle = 90;
			angleEnum = PREVIEW_ROTATION_ANGLE.ROT_90;
			break;
			
		case 1:	// Landscape left
			displayAngle=0;
			angleEnum = PREVIEW_ROTATION_ANGLE.ROT_0;
			break;
			
		case 2:  // Device upside down
			displayAngle=270;
			angleEnum = PREVIEW_ROTATION_ANGLE.ROT_270; 
			break;
			
		case 3:	// Landscape right
			displayAngle=180;
			angleEnum = PREVIEW_ROTATION_ANGLE.ROT_180;
			break;
		}
				
		cameraObj.setDisplayOrientation(displayAngle);
		
		if(_qcSDKEnabled)		
		{
				
				if(faceProc == null)
				{
					faceProc = FacialProcessing.getInstance();
				}
				
				Parameters params = cameraObj.getParameters();
				Size previewSize = params.getPreviewSize();
				int previewWidth = params.getPreviewSize().width;
	        	int previewHeight = params.getPreviewSize().height;
				Log.d("TAG", "Preview Size = "+ previewWidth + "*"+ previewHeight);
				// Landscape mode - front camera 
				if(this.getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE && !switchCamera)
				{
					faceProc.setFrame(data, previewSize.width, previewSize.height, true, angleEnum );					
				}
				// landscape mode - back camera
				else if(this.getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE && switchCamera)
				{				
					faceProc.setFrame(data, previewSize.width, previewSize.height, false, angleEnum );								
				}
				// Portrait mode - front camera
				else if(this.getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT && !switchCamera)
				{
					faceProc.setFrame(data, previewSize.width, previewSize.height, true, angleEnum );
				}
				// Portrait mode - back camera
				else
				{
					faceProc.setFrame(data, previewSize.width, previewSize.height, false, angleEnum );
				}
				
				numFaces = faceProc.getNumFaces();		// Detecting the number of faces in the frame. 
				
				if(numFaces==0)
				{
					Log.d("TAG", "No Face Detected");
					smile.setChecked(false);
					eyeBlink.setChecked(false);
					gazeAngle.setChecked(false);
					if(drawView!=null)
					{
						preview.removeView(drawView);
						drawView = new DrawView(this, null, false);
				        preview.addView(drawView);
				    }
				}
				else
				{			
					Log.d("TAG", "Face Detected");
					faceArray= faceProc.getFaceData();
					
					if(faceArray == null)
					{
							Log.e("TAG", "Face array is null");
					}
					else
					{	
						int surfaceWidth = mPreview.getWidth();
						int surfaceHeight = mPreview.getHeight();
						faceProc.normalizeCoordinates(surfaceWidth, surfaceHeight);										
						if(faceDetectionButtonPress)
						{
							preview.removeView(drawView);					// Remove the previously created view to avoid unnecessary stacking of Views. 
							drawView = new DrawView(this, faceArray, true);		
					        preview.addView(drawView);
						}
						else
						{
							preview.removeView(drawView);
							drawView = new DrawView(this, null, false);
					        preview.addView(drawView);
						}
						if(perfectModeButtonPress)
						{
							for(int i=0; i<numFaces; i++)
							{
								if(faceArray[i].getSmileValue()<75)
								{
									smileFlag = false;
									smile.setChecked(false);
								}
								else
								{
									smile.setChecked(true);
								}
								if(faceArray[i].getLeftEyeBlink()>50 && faceArray[i].getRightEyeBlink()>50)
								{
									blinkFlag=false;
									eyeBlink.setChecked(false);
								}
								else
								{
									eyeBlink.setChecked(true);
								}
								if(faceArray[i].getEyeHorizontalGazeAngle()<-8 || faceArray[i].getEyeHorizontalGazeAngle()>8)
								{
									horizontalGazeAngleFlag = false;
									gazeAngle.setChecked(false);
								}
								else if(faceArray[i].getEyeVerticalGazeAngle()<-8 || faceArray[i].getEyeVerticalGazeAngle()>8)
								{
									verticalGazeAngleFlag = false;
									gazeAngle.setChecked(false);
								}
								else
								{
									gazeAngle.setChecked(true);
								}
								
							}
							if(smileFlag && blinkFlag && horizontalGazeAngleFlag && verticalGazeAngleFlag && cameraButtonPress)
							{						
								try{
								cameraObj.takePicture(shutterCallback, rawCallback, jpegCallback);
								}
								catch(Exception e){
									
								}
								
								frameAnimation.stop();
								cameraButton.setBackgroundResource(R.drawable.shutter_button);
								cameraButton.invalidate();
								cameraButtonPress = false;
								animationPress=false;					
								smile.setVisibility(View.INVISIBLE);
								gazeAngle.setVisibility(View.INVISIBLE);
								eyeBlink.setVisibility(View.INVISIBLE);
								smile.setChecked(false);
								eyeBlink.setChecked(false);
								gazeAngle.setChecked(false);
							}	
						}				
					}					
				}		
		}
		
	}
	

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	/*
	 * (non-Javadoc)
	 * This piece of code handles the view of photos from the gallery button. It decides on what to view when the activity changes its intent. 
	 */
	@Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		switch(requestCode){
		
		case 0:
			if (resultCode == RESULT_OK) {
		        if (requestCode == 0) {
		            Uri selectedImageUri = data.getData();
					Intent intent = new Intent();
	                intent.setType("image/*");
	                intent.setAction(Intent.ACTION_VIEW);
	                intent.setData(selectedImageUri);
	                startActivity(intent);
		        }
		    } 
		    break;
		  // For the rest don't do anything. 
		}	
	}
	

	

	

}
