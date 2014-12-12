/*
 * =========================================================================
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 * =========================================================================
 * @file ChooseUser.java
 */

package com.qualcomm.snapdragon.sdk.recognition.sample;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map.Entry;
import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.view.Menu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.GridView;
import android.widget.Toast;

public class ChooseUser extends Activity {
	
	private GridView gridView;
	private String[] names;
	private boolean deleteUser = false;
	private boolean updateUser = false;
	private HashMap<String, String> hash;
	private static final String ALBUM_NAME = "serialize_deserialize";
	private byte[] albumBuffer;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_choose_user);
		Bundle extras = this.getIntent().getExtras();
		deleteUser = extras.getBoolean("DeleteUser");
		updateUser = extras.getBoolean("UpdateUser");
		gridView = (GridView) findViewById(R.id.listview);
		final FacialRecognitionActivity faceRecog = new FacialRecognitionActivity();
		hash = faceRecog.retrieveHash(getApplicationContext()); // Retrieve the latest hash for display
		refreshUsers();
		
		gridView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
			public void onItemClick(AdapterView<?> parent, View v,
					int position, long id) {
				final String username = (String) parent
						.getItemAtPosition(position); // Get the username associated with the clicked cell.
				
				if (deleteUser && !updateUser) // Delete the user
				{
					new AlertDialog.Builder(ChooseUser.this)
							.setMessage(
									"Are you sure you want to DELETE "
											+ username + " from the album?")
							.setCancelable(true)
							.setPositiveButton("No", null)
							.setNegativeButton("Yes",
									new DialogInterface.OnClickListener() {
										public void onClick(
												DialogInterface dialog, int id) {
											String keyValue = hash
													.get(username);
											// Getting the faceId of the registered user and converting it to integer.
											int faceId = Integer
													.parseInt(keyValue);
											FacialRecognitionActivity.faceObj
													.deletePerson(faceId); // Deleting the user from the database
											saveAlbum();
											hash.remove(username);
											refreshUsers();
											faceRecog.saveHash(hash,
													getApplicationContext());
											Toast.makeText(
													getApplicationContext(),
													username
															+ " deleted successfully.",
													Toast.LENGTH_LONG).show();
										}
									}).show();
				} else if (updateUser && !deleteUser) // Update the user.
				{
					Intent intent = new Intent(ChooseUser.this, AddPhoto.class);
					intent.putExtra("Username", username);
					intent.putExtra("PersonId", hash.get(username));
					intent.putExtra("UpdatePerson", true);
					startActivity(intent);
				}
			}
		});
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.choose_user, menu);
		return true;
	}
	
	/*
	 * Method to display the users in the view
	 */
	private void refreshUsers() {
		
		names = new String[hash.size()];
		int i = 0;
		for (Entry<String, String> entry : hash.entrySet()) {
			names[i] = entry.getKey();
			i++;
		}
		gridView.setAdapter(new UsernameAdapter(this, names));
		
	}
	
	private void saveAlbum() {
		albumBuffer = FacialRecognitionActivity.faceObj
				.serializeRecogntionAlbum();
		SharedPreferences settings = getSharedPreferences(ALBUM_NAME, 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putString("myByteArray", Arrays.toString(albumBuffer));
		editor.commit();
	}
	
}
