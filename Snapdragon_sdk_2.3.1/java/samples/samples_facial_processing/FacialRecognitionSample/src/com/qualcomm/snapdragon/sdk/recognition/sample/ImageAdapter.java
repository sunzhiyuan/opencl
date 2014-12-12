/*
 * =========================================================================
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 * =========================================================================
 * @file ImageAdapter.java
 */

package com.qualcomm.snapdragon.sdk.recognition.sample;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class ImageAdapter extends BaseAdapter {
	
	private Context mContext;
	
	public ImageAdapter(Context context) {
		mContext = context;
	}
	
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		
		View gridView;
		LayoutInflater inflater = (LayoutInflater) mContext
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		if (convertView == null) { // if it's not recycled, initialize some
									// attributes
			
			gridView = new View(mContext);
			gridView = inflater.inflate(R.layout.images, null);
			
		} else {
			gridView = (View) convertView;
		}
		
		ImageView imageView = (ImageView) gridView.findViewById(R.id.imageView);
		imageView.setImageResource(mThumbIds[position]);
		imageView.setScaleType(ImageView.ScaleType.FIT_XY);
		TextView tv = (TextView) gridView.findViewById(R.id.textView);
		tv.setBackgroundColor(Color.BLACK);
		tv.setText(texts[position]);
		
		return gridView;
	}
	
	@Override
	public int getCount() {
		return mThumbIds.length;
	}
	
	@Override
	public Object getItem(int position) {
		return null;
	}
	
	@Override
	public long getItemId(int position) {
		return 0;
	}
	
	// references to our images
	private Integer[] mThumbIds = { R.drawable.add_user,
			R.drawable.update_user, R.drawable.identify_user,
			R.drawable.live_recognition, R.drawable.reset_album,
			R.drawable.delete_user, };
	
	// references to our images
	private String[] texts = { "Add New User", "Update Existing Person",
			"Identify People", "Live Recogniton", "Reset Album",
			"Delete Existing User",
	
	};
	
}
