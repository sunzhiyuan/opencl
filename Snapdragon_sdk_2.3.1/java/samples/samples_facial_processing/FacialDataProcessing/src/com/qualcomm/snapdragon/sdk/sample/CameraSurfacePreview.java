/*
 * ======================================================================
 * Copyright © 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * QTI Proprietary and Confidential.
 * =====================================================================
 * @file: CameraSurfacePreview.java
 */

package com.qualcomm.snapdragon.sdk.sample;

import java.io.IOException;
import java.util.List;

import android.annotation.TargetApi;
import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.os.Build;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.qualcomm.snapdragon.sdk.face.FacialProcessing;

public class CameraSurfacePreview extends SurfaceView implements SurfaceHolder.Callback {

    private final SurfaceHolder mHolder;
    private final Camera mCamera;
    Context mContext;
    FacialProcessing mFaceProc;
    private final int MAX_NUM_BYTES = 1500000;      // Each image is supported for display, upto 1.5 Mb = 1572864 bytes.

    // Facial Values
    int numFaces = 0;
    int smileValue = 0;

    // Supported Preview Sizes for the camera
    int mSupportedWidth;
    int mSupportedHeight;

    @SuppressWarnings("deprecation")
    public CameraSurfacePreview(Context context, Camera camera, FacialProcessing faceProc) {
        super(context);
        mCamera = camera;
        mContext = context;
        // Install a SurfaceHolder.Callback so we get notified when the underlying surface is created and destroyed.
        mHolder = getHolder();
        mHolder.addCallback(this);
        mFaceProc = faceProc;
        Camera.Parameters pm = mCamera.getParameters();

        int index = 0;
        List<Size> previewSize = pm.getSupportedPreviewSizes();
        for (int i = 0; i < previewSize.size(); i++) {
            int width = previewSize.get(i).width;
            int height = previewSize.get(i).height;
            int size = width * height * 3 / 2;
            if (size < MAX_NUM_BYTES) {
                index = i;
                break;
            }
        }
        // Optimal Preview size
        pm.setPreviewSize(previewSize.get(index).width, previewSize.get(index).height);
        mCamera.setParameters(pm);
        // deprecated setting, but required on Android versions prior to 3.0
        mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }

    @Override
    public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {

    }

    @TargetApi(Build.VERSION_CODES.ICE_CREAM_SANDWICH)
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        // The Surface has been created, now tell the camera where to draw the preview.
        try {
            mCamera.setPreviewDisplay(holder);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        mCamera.startPreview();

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

}
