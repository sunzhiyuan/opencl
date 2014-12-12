package com.qualcomm.common;


import android.os.Bundle;
import android.annotation.TargetApi;
import android.app.NativeActivity;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.IOException;
import android.graphics.Color;
import java.nio.ByteBuffer;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;

/**
 * AdrenoNativeActivity class extends NativeActivity and takes over the
 * screen GUI to output the resulting log messages from the application to
 * a TextView on the screen.
 */
@TargetApi(9)
public class AdrenoNativeActivity extends NativeActivity {

	private final static String TAG = "AdrenoNativeActivity";
	private ScrollView mLogCatScrollView = null;
	private TextView mLogCatView = null;
    private LinearLayout mUIView = null;
    private StringBuilder mAppOutput = null;
    private String mImgID = null;
    private String[] mImgPath;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        try
        {
        	ActivityInfo app = getPackageManager().getActivityInfo(this.getComponentName(), PackageManager.GET_ACTIVITIES | PackageManager.GET_META_DATA);
        	Bundle bundle = app.metaData;

        	// Store the name of the package
        	boolean openGLGUI = bundle.getBoolean("opengl_gui");

        	if (!openGLGUI)
        	{
        		// Only provide a GUI if this is an OpenCL-only app (OpenGL apps
        		// do not need a gui)
		        getWindow().takeSurface(null);
		        getWindow().setContentView(R.layout.activity_adreno_native);

		        mAppOutput = new StringBuilder();
		        mLogCatView = (TextView) findViewById(R.id.logCatView);
		        mLogCatView.setText("Running, please wait...");
		        mLogCatScrollView = (ScrollView) findViewById(R.id.logCatScrollView);

				mUIView = (LinearLayout) findViewById(R.id.uiView);

				for (int i = 1; i <= 4; ++i)
				{
					String labelStr = bundle.getString("imageLabel" + i);
					int resID = getResources().getIdentifier("textView"+i, "id", getPackageName());
					TextView label = (TextView) findViewById(resID);
					if (labelStr != null)
					{
						label.setText(labelStr);
					}
				}

				mImgPath = new String[4];

        	}
        	else
        	{
        		// Let OpenGL provide the GUI
        		setContentView(R.layout.activity_adreno_native);
        	}

        }
        catch(Exception e)
        {
        	Log.e(TAG, "Error initializing application", e);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_adreno_native, menu);
        return true;
    }

    ///
    //	logMessage()
    //
    //		This method is called from the Native C++ code in FrmLogMessage()
    //		across the native/JVM boundary.  Each log message is written to the
    //		logcat and also captured here for display.  See the function
    //		Android/FrmUtils_Platform.cpp FrmAdrenoNativeActivityLog
    public void logMessage(String msg)
    {
    	if (mAppOutput != null)
    	{
    		mAppOutput.append(msg);

    		if (mLogCatView != null )
    		{
    			mLogCatView.post(new Runnable() {
    				public void run()
    				{
    					try
    					{
    						if (mLogCatView != null)
    							mLogCatView.setText(mAppOutput);
    						if (mLogCatScrollView != null)
    							mLogCatScrollView.fullScroll(View.FOCUS_DOWN);
    					}
    					catch(Exception e)
    					{
    						Log.e(TAG, "Problem updating logCatView", e);
    					}
    				}
    			});
    		}

		}
    }

	protected Bitmap loadUncompRGB32_TGA(String path) throws IOException
	{
		File file = new File(path);
		InputStream in = null;
		Bitmap bmap = null;
		try {
			in = new BufferedInputStream(new FileInputStream(file));
			in.skip(12);
			int width = (in.read() & 0xff) | ((in.read() & 0xff) << 8);
			int height = (in.read() & 0xff) | ((in.read() & 0xff) << 8);
			int pixelDepth = in.read() & 0xff;
			int size = width * height;

			in.skip(1);
			int pixelSize = pixelDepth / 8;
			int numbytes = size * pixelSize;
			byte buffer[] = new byte[numbytes];
			int buf32[] = new int[size];
			int numbyte = in.read(buffer, 0, numbytes);

			Log.w(TAG, "load image " + path + " w:" + width + " h:"+height + " bytes:" +numbyte );

			// color buffer
			int i;
			for (i = 0; i < size; i++) {
				int offset = i * pixelSize;
				buf32[i] = Color.argb(255, buffer[offset + 2] & 0xff, buffer[offset + 1] & 0xff, buffer[offset ] & 0xff);
            }

			bmap = Bitmap.createBitmap(buf32, width, height, Config.ARGB_8888);

			// tga image needs to be flipped to display on screen
			Bitmap flip = Bitmap.createBitmap(bmap.getWidth(), bmap.getHeight(), bmap.getConfig());
	        Canvas canvas = new Canvas(flip);
	        Paint paint = new Paint();
	        Matrix matrix = new Matrix();
	        matrix.setScale(1, -1);
	        matrix.postTranslate(0, flip.getHeight());
        	canvas.drawBitmap(bmap, matrix, paint);
			bmap = flip;

		}
		catch (IOException e) {
    		Log.e(TAG, "Caught IOException", e);
		}
		finally {
			if (in != null) {
				in.close();
			}
			return bmap;
		}
	}

	///
    //	updateImage()
    //
    //		This method is called from the Native C++ code in FrmAndroidUpdateImage()
    //		to update the image display
    public void updateImage(int index, String path)
    {
        mImgPath[index] = path;

        if (mUIView != null)
        {
            mUIView.post(new Runnable() {
                public void run()
                {
                    try
                    {
                        int i = 0;
                        for (i = 0; i < 4; ++i)
                        {
                            if (mImgPath[i] != null)
                            {
                                int resID = getResources().getIdentifier("imageView"+(i+1), "id", getPackageName());
                                ImageView img = (ImageView) findViewById(resID);
                                Bitmap bMap = loadUncompRGB32_TGA(mImgPath[i]);
                                img.setImageBitmap(bMap);

                                if (i == 0 || i == 1)
                                {
                                    LinearLayout l = (LinearLayout) findViewById(R.id.imgLayoutTop);
                                    l.setVisibility(View.VISIBLE);
                                }
                                else
                                {
                                    LinearLayout l = (LinearLayout) findViewById(R.id.imgLayoutBottom);
                                    l.setVisibility(View.VISIBLE);
                                }
                            }
                        }
                    }
                    catch(Exception e)
                    {
                        Log.e(TAG, "Problem updating image", e);
                    }
                }
            });
        }
    }
}
