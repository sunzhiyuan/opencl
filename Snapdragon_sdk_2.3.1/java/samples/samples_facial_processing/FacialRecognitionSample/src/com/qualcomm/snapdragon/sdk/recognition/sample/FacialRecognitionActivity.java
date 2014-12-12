/*
 * =========================================================================
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 * =========================================================================
 * @file FacialRecognitionActivity.java
 */

package com.qualcomm.snapdragon.sdk.recognition.sample;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import com.qualcomm.snapdragon.sdk.face.FacialProcessing;
import com.qualcomm.snapdragon.sdk.face.FacialProcessing.FEATURE_LIST;
import com.qualcomm.snapdragon.sdk.face.FacialProcessing.FP_MODES;
import android.os.Bundle;
import android.os.Vibrator;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.GridView;
import android.widget.Toast;

public class FacialRecognitionActivity extends Activity {
	
	private GridView gridView;
	public static FacialProcessing faceObj;
	public final String TAG = "FacialRecognitionActivity";
	public final int confidence_value = 58;
	public static boolean activityStartedOnce = false;
	public static final String ALBUM_NAME = "serialize_deserialize";
	public static final String HASH_NAME = "HashMap";
	HashMap<String, String> hash;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_facial_recognition);
		
		hash = retrieveHash(getApplicationContext()); // Retrieve the previously
														// saved Hash Map.
		
		if (!activityStartedOnce) // Check to make sure FacialProcessing object
									// is not created multiple times.
		{
			activityStartedOnce = true;
			// Check if Facial Recognition feature is supported in the device
			boolean isSupported = FacialProcessing
					.isFeatureSupported(FEATURE_LIST.FEATURE_FACIAL_RECOGNITION);
			if (isSupported) {
				Log.d(TAG, "Feature Facial Recognition is supported");
				faceObj = (FacialProcessing) FacialProcessing.getInstance();
				loadAlbum(); // De-serialize a previously stored album.
				if (faceObj != null) {
					faceObj.setRecognitionConfidence(confidence_value);
					faceObj.setProcessingMode(FP_MODES.FP_MODE_STILL);
				}
			} else // If Facial recognition feature is not supported then
					// display an alert box.
			{
				Log.e(TAG, "Feature Facial Recognition is NOT supported");
				new AlertDialog.Builder(this)
						.setMessage(
								"Your device does NOT support Qualcomm's Facial Recognition feature. ")
						.setCancelable(false)
						.setNegativeButton("OK",
								new DialogInterface.OnClickListener() {
									public void onClick(DialogInterface dialog,
											int id) {
										FacialRecognitionActivity.this.finish();
									}
								}).show();
			}
		}
		// Vibrator for button press
		final Vibrator vibrate = (Vibrator) FacialRecognitionActivity.this
				.getSystemService(Context.VIBRATOR_SERVICE);
		
		gridView = (GridView) findViewById(R.id.gridview);
		gridView.setAdapter(new ImageAdapter(this));
		
		gridView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
			public void onItemClick(AdapterView<?> parent, View v,
					int position, long id) {
				vibrate.vibrate(85);
				switch (position) {
				
				case 0: // Adding a person
					addNewPerson();
					break;
				
				case 1: // Updating an existing person
					updateExistingPerson();
					break;
				
				case 2: // Identifying a person.
					identifyPerson();
					break;
				
				case 3: // Live Recognition
					liveRecognition();
					break;
				
				case 4: // Reseting an album
					resetAlbum();
					break;
				
				case 5: // Delete Existing Person
					deletePerson();
					break;
				
				}
			}
		});
	}
	
	/*
	 * Method to handle adding a new person to the recognition album
	 */
	private void addNewPerson() {
		Intent intent = new Intent(this, AddPhoto.class);
		intent.putExtra("Username", "null");
		intent.putExtra("PersonId", -1);
		intent.putExtra("UpdatePerson", false);
		intent.putExtra("IdentifyPerson", false);
		startActivity(intent);
	}
	
	/*
	 * Method to handle updating of an existing person from the recognition
	 * album
	 */
	private void updateExistingPerson() {
		Intent intent = new Intent(this, ChooseUser.class);
		intent.putExtra("DeleteUser", false);
		intent.putExtra("UpdateUser", true);
		startActivity(intent);
	}
	
	/*
	 * Method to handle identification of an existing person from the
	 * recognition album
	 */
	private void identifyPerson() {
		Intent intent = new Intent(this, AddPhoto.class);
		intent.putExtra("Username", "Not Identified");
		intent.putExtra("PersonId", -1);
		intent.putExtra("UpdatePerson", false);
		intent.putExtra("IdentifyPerson", true);
		startActivity(intent);
	}
	
	/*
	 * Method to handle deletion of an existing person from the recognition
	 * album
	 */
	private void deletePerson() {
		Intent intent = new Intent(this, ChooseUser.class);
		intent.putExtra("DeleteUser", true);
		intent.putExtra("UpdateUser", false);
		startActivity(intent);
	}
	
	/*
	 * Method to handle live identification of the people
	 */
	private void liveRecognition() {
		Intent intent = new Intent(this, LiveRecognition.class);
		startActivity(intent);
	}
	
	/*
	 * Method to handle reseting of the recognition album
	 */
	private void resetAlbum() {
		// Alert box to confirm before reseting the album
		new AlertDialog.Builder(this)
				.setMessage(
						"Are you sure you want to RESET the album? All the photos saved will be LOST")
				.setCancelable(true)
				.setNegativeButton("No", null)
				.setPositiveButton("Yes",
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								boolean result = faceObj.resetAlbum();
								if (result) {
									HashMap<String, String> hashMap = retrieveHash(getApplicationContext());
									hashMap.clear();
									saveHash(hashMap, getApplicationContext());
									saveAlbum();
									Toast.makeText(getApplicationContext(),
											"Album Reset Successful.",
											Toast.LENGTH_LONG).show();
								} else {
									Toast.makeText(
											getApplicationContext(),
											"Internal Error. Reset album failed",
											Toast.LENGTH_LONG).show();
								}
							}
						}).show();
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.facial_recognition, menu);
		return true;
	}
	
	protected void onPause() {
		super.onPause();
	}
	
	protected void onDestroy() {
		super.onDestroy();
		Log.d(TAG, "Destroyed");
		if (faceObj != null) // If FacialProcessing object is not released, then
								// release it and set it to null
		{
			faceObj.release();
			faceObj = null;
			Log.d(TAG, "Face Recog Obj released");
		} else {
			Log.d(TAG, "In Destroy - Face Recog Obj = NULL");
		}
	}
	
	@Override
	protected void onStop() {
		super.onStop();
	}
	
	protected void onResume() {
		super.onResume();
	}
	
	@Override
	public void onBackPressed() { // Destroy the activity to avoid stacking of
									// android activities
		super.onBackPressed();
		FacialRecognitionActivity.this.finishAffinity();
		activityStartedOnce = false;
	}
	
	/*
	 * Function to retrieve a HashMap from the Shared preferences.
	 * @return
	 */
	protected HashMap<String, String> retrieveHash(Context context) {
		SharedPreferences settings = context.getSharedPreferences(HASH_NAME, 0);
		HashMap<String, String> hash = new HashMap<String, String>();
		hash.putAll((Map<? extends String, ? extends String>) settings.getAll());
		return hash;
	}
	
	/*
	 * Function to store a HashMap to shared preferences.
	 * @param hash
	 */
	protected void saveHash(HashMap<String, String> hashMap, Context context) {
		SharedPreferences settings = context.getSharedPreferences(HASH_NAME, 0);
		
		SharedPreferences.Editor editor = settings.edit();
		editor.clear();
		Log.e(TAG, "Hash Save Size = " + hashMap.size());
		for (String s : hashMap.keySet()) {
			editor.putString(s, hashMap.get(s));
		}
		editor.commit();
	}
	
	/*
	 * Function to retrieve the byte array from the Shared Preferences.
	 */
	public void loadAlbum() {
		SharedPreferences settings = getSharedPreferences(ALBUM_NAME, 0);
		String arrayOfString = settings.getString("albumArray", null);
		
		byte[] albumArray = null;
		if (arrayOfString != null) {
			String[] splitStringArray = arrayOfString.substring(1,
					arrayOfString.length() - 1).split(", ");
			
			albumArray = new byte[splitStringArray.length];
			for (int i = 0; i < splitStringArray.length; i++) {
				albumArray[i] = Byte.parseByte(splitStringArray[i]);
			}
			faceObj.deserializeRecognitionAlbum(albumArray);
			Log.e("TAG", "De-Serialized my album");
		}
	}
	
	/*
	 * Method to save the recognition album to a permanent device memory
	 */
	public void saveAlbum() {
		byte[] albumBuffer = faceObj.serializeRecogntionAlbum();
		SharedPreferences settings = getSharedPreferences(ALBUM_NAME, 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putString("albumArray", Arrays.toString(albumBuffer));
		editor.commit();
	}
	
}
