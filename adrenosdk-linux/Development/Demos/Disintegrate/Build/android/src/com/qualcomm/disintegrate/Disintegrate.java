//--------------------------------------------------------------------------------------
// Author: QUALCOMM, Advanced Content Group
// Copyright (c) 2013 QUALCOMM Technologies, Inc.
// All Rights Reserved. 
// QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

package com.qualcomm.disintegrate;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Process;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.content.res.AssetManager;

//=============================================================================
public class Disintegrate extends Activity implements
		GLSurfaceView.EGLConfigChooser, GLSurfaceView.EGLContextFactory,
		GLSurfaceView.Renderer
//=============================================================================
{
    //*******************************************
    // Keycode values that match the framework
    //*******************************************
    private static final int ADRENO_KEY_1       = (1 <<  0);
    private static final int ADRENO_KEY_2       = (1 <<  1);
    private static final int ADRENO_KEY_3       = (1 <<  2);
    private static final int ADRENO_KEY_4       = (1 <<  3);
    private static final int ADRENO_KEY_5       = (1 <<  4);
    private static final int ADRENO_KEY_6       = (1 <<  5);
    private static final int ADRENO_KEY_7       = (1 <<  6);
    private static final int ADRENO_KEY_8       = (1 <<  7);
    private static final int ADRENO_KEY_9       = (1 <<  8);
    private static final int ADRENO_KEY_0       = (1 <<  9);
    private static final int ADRENO_KEY_STAR    = (1 << 10);
    private static final int ADRENO_DPAD_UP     = (1 << 17);
    private static final int ADRENO_DPAD_DOWN   = (1 << 18);
    private static final int ADRENO_DPAD_LEFT   = (1 << 19);
    private static final int ADRENO_DPAD_RIGHT  = (1 << 20);


    boolean gAltPressed = false;
	
	private static final String LOG_TAG = "Disintegrate";
	private static final String STATE_TAG_POINTER = "com.qualcomm.disintegrate:mNativeObjectPointer";
	private static final String STATE_TAG_PID = "com.qualcomm.disintegrate:pid";

	private static native void nativeLoadAssets(AssetManager mgr);
	private AssetManager m_assetMgr;

	/**
	 * EGL constants introduced in EGL 1.2 or later, required to set up a surface
	 * compatible with OpengGL|ES 2.0
	 */
	private static final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
	private static final int EGL_OPENGL_ES2_BIT = 0x0004;

	/**
	 * Attributes to pass to eglCreateConfig for a basic render buffer.  These
	 * values can be increased for higher pixel depth, alpha or stencil buffers,
	 * or multisample antialiasing.
	 */
	private static final int[] CONFIG_SPEC = {
		EGL10.EGL_RED_SIZE,		5,
		EGL10.EGL_GREEN_SIZE,	6,
		EGL10.EGL_BLUE_SIZE,	5,
		EGL10.EGL_ALPHA_SIZE,	0,
		EGL10.EGL_DEPTH_SIZE,	16,
		EGL10.EGL_STENCIL_SIZE,	0,
		EGL10.EGL_SAMPLE_BUFFERS,0,
		EGL10.EGL_SAMPLES,       0,
		EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL10.EGL_NONE
	};

	/**
	 * Attributes to pass to eglCreateContext
	 */
	private static final int[] CONTEXT_SPEC = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL10.EGL_NONE
	};

	/**
	 * Keep a reference to the active GLSurfaceView so pause and resume events
	 * can be passed to it
	 */
	private GLSurfaceView       mSurfaceView;

	/**
	 * In native code, create a class object holding all the important application
	 * information. Keep a copy of that pointer in Java to global state in native.
	 */
	private int					mNativeObjectPointer;

	/**
	 * Native function: Allocate and initialize any necessary native memory. To
	 * persist data, allocate a single structure and return its pointer.
	 *
	 * @return A pointer to a native structure, or NULL on error
	 */
	protected final native int			nativeAllocate();

	/**
	 * Native functon: Initialize any native graphics state.
	 *
	 * @param nativeObject
	 *            The pointer returned by nativeAllocate()
	 * @return True on success
	 */
	protected final native boolean		nativeInitialize(int nativeObject);

	/**
	 * Native function: The size of the GLES surface has changed. Make any state
	 * changes necessary to account for the new size.
	 *
	 * @param nativeObject
	 *            The pointer returned by nativeInitialize()
	 * @param width
	 *            The new width of the surface
	 * @param height
	 *            The new height of the surface
	 * @return True on success
	 */
	protected final native boolean		nativeSetSize(int nativeObject, int width, int height);

	/**
	 * Native function: Render a single frame, but do not call eglSwapBuffers.
	 *
	 * @param nativeObject
	 *            The pointer returned by nativeInitialize
	 * @return True to continue rendering, false to exit the application
	 */
	protected final native boolean		nativeRender(int nativeObject);

	/**
	 * Native function: Clean up all native graphics state
	 *
	 * @param nativeObject
	 *            The pointer returned by nativeInitialize
	 * @return True on success
	 */
	protected final native boolean		nativeDestroy(int nativeObject);

	/**
	 * Native function: Free the allocated native memory
	 *
	 * @param nativeObject
	 *            The pointer returned by nativeInitialize
	 * @return True on success
	 */
	protected final native boolean		nativeFree(int nativeObject);

	/**
	 * Native function: Register all other native functions.  This function will
	 * be looked up by name, so must be compiled with C linkage.  By calling
	 * JNI RegisterNatives here, other native functions may be compiled with
	 * C++ linkage.
	 */
	protected static final native void	registerNativeFns();


	/**
	 * Native function: Called when a key down event has occurred
	 *
	 * @param nativeObject
	 *            The pointer returned by nativeInitialize()
	 * @param keyode
	 *            The key pressed
	 * @return True on success
	 */
	protected final native boolean		nativeKeyDown(int nativeObject, int keyCode);

	/**
	 * Native function: Called when a key up event has occurred
	 *
	 * @param nativeObject
	 *            The pointer returned by nativeInitialize()
	 * @param keyode
	 *            The key pressed
	 * @return True on success
	 */
	protected final native boolean		nativeKeyUp(int nativeObject, int keyCode);

	/**
	 * Native function: Called when screen is touched
	 *
	 * @param nativeObject
	 *            The pointer returned by nativeInitialize()
	 * @param xPos
	 *            X position of the touch
	 * @param yPos
	 *            Y position of the touch
	 * @return True on success
	 */
	protected final native boolean		nativeTouchEvent(int nativeObject, int xPos, int yPos);

	/**
	 * Load the native library as soon as possible, and exit as completely as
	 * possible if it fails.
	 */
	static {
		try {
			System.loadLibrary("Disintegrate");
			registerNativeFns();
		} catch (UnsatisfiedLinkError e){
			Log.e(LOG_TAG, "Could not load native library");
			e.printStackTrace();
			System.exit(1);
		}
	}

	/**
	 * Request a fullscreen app, and create a GLSurfaceView to render into.
	 *
     * @see android.app.Activity#onCreate(android.os.Bundle)
     */
    @Override
	//-------------------------------------------------------------------------
    public void onCreate(Bundle savedInstanceState)
	//-------------------------------------------------------------------------
    {
        super.onCreate(savedInstanceState);

        if (savedInstanceState != null) {
        	if (savedInstanceState.getInt(STATE_TAG_PID) == Process.myPid()) {
        		mNativeObjectPointer = savedInstanceState.getInt(STATE_TAG_POINTER);
        	}
        }

        setTheme(android.R.style.Theme_NoTitleBar_Fullscreen);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        mSurfaceView = new GLSurfaceView(this);
        mSurfaceView.setEGLConfigChooser(this);
        mSurfaceView.setEGLContextFactory(this);
        mSurfaceView.setRenderer(this);
        setContentView(mSurfaceView);

		m_assetMgr = getResources().getAssets();
		nativeLoadAssets(m_assetMgr);
    }

    @Override
	//-------------------------------------------------------------------------
    protected void onResume()
	//-------------------------------------------------------------------------
    {
       	mSurfaceView.onResume();
    	super.onResume();
    }

    /**
	 * Not only do you have to create the EGL config object, you have to set the
	 * correct format on the SurfaceHolder that our GLSurfaceView is using.
	 * Failure to do so results in crashes on some devices. This implementation
	 * assumes the driver will return its preferred configuration first, so only
	 * requests one, and queries its attributes to pick the right PixelFormat.
	 *
	 * @see android.opengl.GLSurfaceView.EGLConfigChooser#chooseConfig(EGL10, EGLDisplay)
	 */
	@Override
	//-------------------------------------------------------------------------
	public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
	//-------------------------------------------------------------------------
	{
		final int MAX_CONFIGS = 16;
		EGLConfig[] configs = new EGLConfig[MAX_CONFIGS];
		int[] num_config = new int[1];

		egl.eglChooseConfig(display, CONFIG_SPEC, configs, MAX_CONFIGS, num_config);
		int bestDepth = 99999;
		EGLConfig bestConfig = null;
		int[] attrib = new int[1];
		for (int i = 0; i < num_config[0]; i++) {
			EGLConfig config = configs[i];
			int depth = 0;
			
			egl.eglGetConfigAttrib(display, config, EGL10.EGL_RED_SIZE, attrib);
			depth += attrib[0];
			egl.eglGetConfigAttrib(display, config, EGL10.EGL_GREEN_SIZE, attrib);
			depth += attrib[0];
			egl.eglGetConfigAttrib(display, config, EGL10.EGL_BLUE_SIZE, attrib);
			depth += attrib[0];
			egl.eglGetConfigAttrib(display, config, EGL10.EGL_ALPHA_SIZE, attrib);
			depth += attrib[0];
			egl.eglGetConfigAttrib(display, config, EGL10.EGL_DEPTH_SIZE, attrib);
			depth += attrib[0];
			egl.eglGetConfigAttrib(display, config, EGL10.EGL_STENCIL_SIZE, attrib);
			depth += attrib[0];
			
			egl.eglGetConfigAttrib(display, config, EGL10.EGL_SAMPLE_BUFFERS, attrib);
			int ms_buffers = attrib[0];
			egl.eglGetConfigAttrib(display, config, EGL10.EGL_SAMPLES, attrib);
			depth += depth * ms_buffers * attrib[0];
			
			egl.eglGetConfigAttrib(display, config, EGL10.EGL_CONFIG_CAVEAT, attrib);
			if (attrib[0] != EGL10.EGL_NONE)
				// Seriously penalize slow or nonconformant configs
				depth += 9999;
			
			if (depth < bestDepth) {
				bestDepth = depth;
				bestConfig = config;
			}
		}

		return bestConfig;
	}

	/**
	 * Create a dedicated OpenGL|ES 2.0 context.
	 *
	 * @see android.opengl.GLSurfaceView.EGLContextFactory#createContext(EGL10, EGLDisplay, EGLConfig)
	 */
	@Override
	//-------------------------------------------------------------------------
	public EGLContext createContext(EGL10 egl, EGLDisplay display,
			EGLConfig eglConfig)
	//-------------------------------------------------------------------------
	{
		return egl.eglCreateContext(display, eglConfig,EGL10.EGL_NO_CONTEXT, CONTEXT_SPEC);
	}

	/**
	 * Once the surface is created, initialize the native state.
	 *
	 * @see android.opengl.GLSurfaceView.Renderer#onSurfaceCreated(GL10, EGLConfig)
	 */
	@Override
	//-------------------------------------------------------------------------
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	//-------------------------------------------------------------------------
	{
    	// Do not call this on the main thread, to avoid blocking the UI on long
		// load times.
    	if (mNativeObjectPointer == 0) {
    		mNativeObjectPointer = nativeAllocate();
		}

		if (!nativeInitialize(mNativeObjectPointer)) {
			Log.e(LOG_TAG, "Error initializing GL state");
		}
	}

	/**
	 * The surface size has changed - this will be called once immediately after
	 * the surface is created, then later if the layout changes.  Call glViewport
	 * here, and anything else that may depend on the aspect ratio of the screen.
	 *
	 * @see android.opengl.GLSurfaceView.Renderer#onSurfaceChanged(GL10, int, int)
	 */
	@Override
	//-------------------------------------------------------------------------
	public void onSurfaceChanged(GL10 gl, int width, int height)
	//-------------------------------------------------------------------------
	{
		if (!nativeSetSize(mNativeObjectPointer, width, height)) {
			Log.e(LOG_TAG, "Error setting size");
		}
	}

	/**
	 * Draw a single frame, but do not call eglSwapBuffers. GLSurfaceView will
	 * take care of that.
	 *
	 * @see android.opengl.GLSurfaceView.Renderer#onDrawFrame(GL10)
	 */
	@Override
	//-------------------------------------------------------------------------
	public void onDrawFrame(GL10 gl)
	//-------------------------------------------------------------------------
	{
		if (!nativeRender(mNativeObjectPointer)) {
			finish();
		}
	}

	@Override
	//-------------------------------------------------------------------------
	protected void onPause()
	//-------------------------------------------------------------------------
	{
		mSurfaceView.onPause();
		super.onPause();
	}

	/**
	 * If you create the context, you are also responsible for destroying it.
	 * Take this opportunity to clean up any native state that is invalid without
	 * a valid context.
	 *
	 * @see android.opengl.GLSurfaceView.EGLContextFactory#destroyContext(EGL10, EGLDisplay, EGLContext)
	 */
	@Override
	//-------------------------------------------------------------------------
	public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context)
	//-------------------------------------------------------------------------
	{
		if (!nativeDestroy(mNativeObjectPointer)) {
			Log.e(LOG_TAG, "Error destroying GL state");
		}
		egl.eglDestroyContext(display, context);
	}

	/**
	 * Save any information that needs to be preserved across destructions of
	 * the Activity. As long as we stay in the same process, this can include
	 * the native memory pointer.
	 *
	 * @see android.app.Activity#onSaveInstanceState(android.os.Bundle)
	 */
	@Override
	protected void onSaveInstanceState(Bundle outState) {
		outState.putInt(STATE_TAG_POINTER, mNativeObjectPointer);
		outState.putInt(STATE_TAG_PID, Process.myPid());
		super.onSaveInstanceState(outState);
	}

	/**
	 * The Activity is being destroyed.  If this is the actual exit of the
	 * application, free all native memory.
	 * @see android.app.Activity#onDestroy()
	 */
	@Override
	//-------------------------------------------------------------------------
	protected void onDestroy()
	//-------------------------------------------------------------------------
	{
    	if (isFinishing()) {
	    	if (!nativeFree(mNativeObjectPointer)) {
	    		Log.e(LOG_TAG, "Error freeing native memory");
	    	}
	    	mNativeObjectPointer = 0;
    	}
		super.onDestroy();
	}
	
    @Override
	//-------------------------------------------------------------------------
    public boolean onKeyDown(final int keyCode, KeyEvent event) 
	//-------------------------------------------------------------------------
    {
    	switch(keyCode)
    	{
    	case KeyEvent.KEYCODE_ALT_LEFT:
    	case KeyEvent.KEYCODE_ALT_RIGHT:
    		gAltPressed = true;
    		return super.onKeyDown(keyCode, event);
    		
    	case KeyEvent.KEYCODE_1: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_1); return true;
    	case KeyEvent.KEYCODE_2: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_2); return true;
    	case KeyEvent.KEYCODE_3: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_3); return true;
    	case KeyEvent.KEYCODE_4: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_4); return true;
    	case KeyEvent.KEYCODE_5: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_5); return true;
    	case KeyEvent.KEYCODE_6: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_6); return true;
    	case KeyEvent.KEYCODE_7: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_7); return true;
    	case KeyEvent.KEYCODE_8: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_8); return true;
    	case KeyEvent.KEYCODE_9: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_9); return true;
    	case KeyEvent.KEYCODE_0: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_0); return true;
    	case KeyEvent.KEYCODE_STAR: nativeKeyDown(mNativeObjectPointer, ADRENO_KEY_STAR); return true;
    	case KeyEvent.KEYCODE_DPAD_UP: nativeKeyDown(mNativeObjectPointer, ADRENO_DPAD_UP); return true;
    	case KeyEvent.KEYCODE_DPAD_DOWN: nativeKeyDown(mNativeObjectPointer, ADRENO_DPAD_DOWN); return true;
    	case KeyEvent.KEYCODE_DPAD_LEFT: nativeKeyDown(mNativeObjectPointer, ADRENO_DPAD_LEFT); return true;
    	case KeyEvent.KEYCODE_DPAD_RIGHT: nativeKeyDown(mNativeObjectPointer, ADRENO_DPAD_RIGHT); return true;
		}

    	// Check if pressing a key while ALT is set
    	int TempCode = 0;
    	if(gAltPressed)
    	{
	    	switch(keyCode)
	    	{
	    	case KeyEvent.KEYCODE_Q: TempCode = ADRENO_KEY_1; break; 
	    	case KeyEvent.KEYCODE_W: TempCode = ADRENO_KEY_2; break; 
	    	case KeyEvent.KEYCODE_E: TempCode = ADRENO_KEY_3; break; 
	    	case KeyEvent.KEYCODE_R: TempCode = ADRENO_KEY_4; break; 
	    	case KeyEvent.KEYCODE_T: TempCode = ADRENO_KEY_5; break; 
	    	case KeyEvent.KEYCODE_Y: TempCode = ADRENO_KEY_6; break; 
	    	case KeyEvent.KEYCODE_U: TempCode = ADRENO_KEY_7; break; 
	    	case KeyEvent.KEYCODE_I: TempCode = ADRENO_KEY_8; break; 
	    	case KeyEvent.KEYCODE_O: TempCode = ADRENO_KEY_9; break; 
	    	case KeyEvent.KEYCODE_P: TempCode = ADRENO_KEY_0; break; 
	    	}
    	}
    	
    	// Did we find a key because ALT was being used?
    	if(TempCode != 0)
    	{
    		nativeKeyDown(mNativeObjectPointer, TempCode); 
    		return true;
    	}

    	// If not trapped above, pass it up to be handled
		return super.onKeyDown(keyCode, event);
    }
	
    @Override
	//-------------------------------------------------------------------------
    public boolean onKeyUp(final int keyCode, KeyEvent event) 
	//-------------------------------------------------------------------------
    {
    	switch(keyCode)
    	{
    	case KeyEvent.KEYCODE_ALT_LEFT:
    	case KeyEvent.KEYCODE_ALT_RIGHT:
    		gAltPressed = false;
    		return super.onKeyDown(keyCode, event);

    	case KeyEvent.KEYCODE_1: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_1); return true;
    	case KeyEvent.KEYCODE_2: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_2); return true;
    	case KeyEvent.KEYCODE_3: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_3); return true;
    	case KeyEvent.KEYCODE_4: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_4); return true;
    	case KeyEvent.KEYCODE_5: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_5); return true;
    	case KeyEvent.KEYCODE_6: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_6); return true;
    	case KeyEvent.KEYCODE_7: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_7); return true;
    	case KeyEvent.KEYCODE_8: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_8); return true;
    	case KeyEvent.KEYCODE_9: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_9); return true;
    	case KeyEvent.KEYCODE_0: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_0); return true;
    	case KeyEvent.KEYCODE_STAR: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_STAR); return true;
    	case KeyEvent.KEYCODE_DPAD_UP: nativeKeyUp(mNativeObjectPointer, ADRENO_DPAD_UP); return true;
    	case KeyEvent.KEYCODE_DPAD_DOWN: nativeKeyUp(mNativeObjectPointer, ADRENO_DPAD_DOWN); return true;
    	case KeyEvent.KEYCODE_DPAD_LEFT: nativeKeyUp(mNativeObjectPointer, ADRENO_DPAD_LEFT); return true;
    	case KeyEvent.KEYCODE_DPAD_RIGHT: nativeKeyUp(mNativeObjectPointer, ADRENO_DPAD_RIGHT); return true;
    	
    	// Can ignore checking for ALT key on the "Up"
    	case KeyEvent.KEYCODE_Q: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_1); return true; 
    	case KeyEvent.KEYCODE_W: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_2); return true; 
    	case KeyEvent.KEYCODE_E: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_3); return true; 
    	case KeyEvent.KEYCODE_R: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_4); return true; 
    	case KeyEvent.KEYCODE_T: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_5); return true; 
    	case KeyEvent.KEYCODE_Y: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_6); return true; 
    	case KeyEvent.KEYCODE_U: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_7); return true; 
    	case KeyEvent.KEYCODE_I: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_8); return true; 
    	case KeyEvent.KEYCODE_O: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_9); return true; 
    	case KeyEvent.KEYCODE_P: nativeKeyUp(mNativeObjectPointer, ADRENO_KEY_0); return true; 
		}

    	// If not trapped above, pass it up to be handled
		return super.onKeyUp(keyCode, event);
    }
	
    @Override
	//-------------------------------------------------------------------------
    public boolean onTouchEvent(final MotionEvent event)
	//-------------------------------------------------------------------------
    {
		if (!nativeTouchEvent(mNativeObjectPointer, (int)event.getX(), (int)event.getY())) 
		{
			Log.e(LOG_TAG, "Error handling touch event");
			return false;
		}
		
		return true;
    }
	
	
}
