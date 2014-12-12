/* ======================================================================
 *  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 *  QTI Proprietary and Confidential.
 *  =====================================================================
 * @file:   GalleryProcessing.java
 *
 */
package com.qualcomm.snapdragon.sdk.sample;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.BitmapFactory.Options;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Bundle;
import android.os.SystemClock;
import android.provider.MediaStore;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.qualcomm.snapdragon.sdk.face.FaceData;
import com.qualcomm.snapdragon.sdk.face.FacialProcessing;

public class GalleryProcessing extends Activity {

	private final int PIC_FROM_GALLERY = 1;
	private long time;
	private FacialProcessing fp;
	private ImageView image;
	private TextView text;
	public final String PROJECTION_PATH = MediaStore.Images.Media.DATA;
	
	//static makes the values stay even during screen rotation
	private static String textToWrite = "";
	private static Bitmap currentDisplayedImage = null;
	private int[] colors = {
			Color.RED, Color.BLUE,
			Color.GREEN, Color.YELLOW,
			Color.CYAN, Color.MAGENTA
	};
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_image_conversion);
		
		image = (ImageView)findViewById(R.id.picture);
		text = (TextView)findViewById(R.id.text);
		text.setTextColor(Color.WHITE);
		
		// This handles keeping the images and text
		// for when the screen is rotated
		if(currentDisplayedImage!=null)
			image.setImageBitmap(currentDisplayedImage);
		
		if(textToWrite.length()>0)
			text.setText(textToWrite);
		
		fp = FacialProcessing.getInstance();
		
		Button selectButton = (Button)findViewById(R.id.select_picture_button);
		selectButton.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				textToWrite = "";
				Intent gallery = new Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
				startActivityForResult(gallery, PIC_FROM_GALLERY);
			}
		});
	}

	@Override
	protected void onDestroy(){
		if(fp!=null) fp.release();
		super.onDestroy();
	}
	
	@Override
	protected void onActivityResult(int requestCode, int finalResultCode, Intent returnedImage){
		super.onActivityResult(requestCode, finalResultCode, returnedImage);
		if (requestCode == PIC_FROM_GALLERY && finalResultCode == RESULT_OK && returnedImage!=null) {
			ContentResolver resolver = getContentResolver();
            Uri userSelectedImage = returnedImage.getData();
            String[] filePathColumn = {PROJECTION_PATH}; 
            Cursor csr = resolver.query(userSelectedImage, filePathColumn, null, null, null);
            csr.moveToFirst();
            int selectedIndex = 0;
            String picturePath = csr.getString(selectedIndex);
            csr.close();
            
            time = SystemClock.currentThreadTimeMillis();
            //make it mutable so we can draw on it later
            Options bitmapOptions = new Options();
            bitmapOptions.inMutable = true;
            Bitmap bmp = BitmapFactory.decodeFile(picturePath, bitmapOptions);
            time = SystemClock.currentThreadTimeMillis()-time;
            
            textToWrite += "Image Dimensions:\n" + bmp.getWidth() +"x" + bmp.getHeight()+"\n";
            textToWrite += "Decode Bitmap:" + time + " ms\n";
            analyzeAndDisplayImage(bmp);
        }
	}
	
	private void analyzeAndDisplayImage(Bitmap bmp){
		time = SystemClock.currentThreadTimeMillis();
		boolean analyzed = fp.setBitmap(bmp);
		time = SystemClock.currentThreadTimeMillis() - time;
		textToWrite += "Process Bitmap:"+time+" ms\n";
		
		if(!analyzed)
			textToWrite+="setBitmapFailed.\n";
		else{
			textToWrite += "Number of Faces:" + fp.getNumFaces()+"\n";
			Paint paint = new Paint();
			Canvas canvas = new Canvas(bmp);
			int x, y;
			float pixelDensity = getResources().getDisplayMetrics().density;
			FaceData[] faces = fp.getFaceData();
			for(int i = 0; i < fp.getNumFaces(); i++){
				paint.setColor(colors[i%6]);
				FaceData f = faces[i];
				
				Rect r = f.rect;
				paint.setStyle(Paint.Style.STROKE);
				paint.setStrokeWidth(r.width()/75*pixelDensity);
				canvas.drawRect(r, paint);
				
				paint.setStyle(Paint.Style.FILL);
				x = f.leftEye.x;
				y = f.leftEye.y;
				canvas.drawCircle(x, y, r.width()/50*pixelDensity,paint);
				x = f.rightEye.x;
				y = f.rightEye.y;
				canvas.drawCircle(x, y, r.width()/50*pixelDensity, paint);
				x = f.mouth.x;
				y = f.mouth.y;
				canvas.drawCircle(x, y, r.width()/25*pixelDensity, paint);
			}
		}
		currentDisplayedImage = bmp;
		image.setImageBitmap(bmp);
		text.setText(textToWrite);
	}
	
}
