/*
 * ======================================================================
 * Copyright © 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * QTI Proprietary and Confidential.
 * =====================================================================
 * @file: DrawView.java
 */

package com.qualcomm.snapdragon.sdk.sample;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.PorterDuff.Mode;
import android.graphics.Rect;
import android.hardware.Camera;
import android.view.SurfaceView;

import com.qualcomm.snapdragon.sdk.face.FaceData;

public class DrawView extends SurfaceView {

    private final Paint leftEyeBrush = new Paint();
    private final Paint rightEyeBrush = new Paint();
    private final Paint mouthBrush = new Paint();
    private final Paint rectBrush = new Paint();
    public Point leftEye, rightEye, mouth;
    Rect mFaceRect;
    public FaceData[] mFaceArray;
    boolean _inFrame;            // Boolean to see if there is any faces in the frame
    int mSurfaceWidth;
    int mSurfaceHeight;
    int cameraPreviewWidth;
    int cameraPreviewHeight;
    boolean mLandScapeMode;
    float scaleX = 1.0f;
    float scaleY = 1.0f;

    public DrawView(Context context, FaceData[] faceArray, boolean inFrame, int surfaceWidth, int surfaceHeight,
            Camera cameraObj, boolean landScapeMode) {
        super(context);

        setWillNotDraw(false);                    // This call is necessary, or else the draw method will not be called.
        mFaceArray = faceArray;
        _inFrame = inFrame;
        mSurfaceWidth = surfaceWidth;
        mSurfaceHeight = surfaceHeight;
        mLandScapeMode = landScapeMode;
        if (cameraObj != null) {
            cameraPreviewWidth = cameraObj.getParameters().getPreviewSize().width;
            cameraPreviewHeight = cameraObj.getParameters().getPreviewSize().height;
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {

        if (_inFrame)                // If the face detected is in frame.
        {
            for (int i = 0; i < mFaceArray.length; i++) {
                if (mFaceArray[i].leftEye != null) {
                    leftEyeBrush.setColor(Color.RED);
                    canvas.drawCircle(mFaceArray[i].leftEye.x * scaleX, mFaceArray[i].leftEye.y * scaleY, 5f,
                            leftEyeBrush);

                    rightEyeBrush.setColor(Color.GREEN);
                    canvas.drawCircle(mFaceArray[i].rightEye.x * scaleX, mFaceArray[i].rightEye.y * scaleY, 5f,
                            rightEyeBrush);

                    mouthBrush.setColor(Color.WHITE);
                    canvas.drawCircle(mFaceArray[i].mouth.x * scaleX, mFaceArray[i].mouth.y * scaleY, 5f, mouthBrush);
                }
                if (mFaceArray[i].leftEyeObj != null) {
                    mouthBrush.setColor(Color.CYAN);
                    canvas.drawCircle(mFaceArray[i].mouthObj.left.x, mFaceArray[i].mouthObj.left.y, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].mouthObj.right.x, mFaceArray[i].mouthObj.right.y, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].mouthObj.upperLipTop.x, mFaceArray[i].mouthObj.upperLipTop.y, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].mouthObj.upperLipBottom.x, mFaceArray[i].mouthObj.upperLipBottom.y,
                            5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].mouthObj.lowerLipTop.x, mFaceArray[i].mouthObj.lowerLipTop.y, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].mouthObj.lowerLipBottom.x, mFaceArray[i].mouthObj.lowerLipBottom.y,
                            5f, mouthBrush);

                    canvas.drawCircle(mFaceArray[i].leftEyebrow.left.x, mFaceArray[i].leftEyebrow.left.y, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].leftEyebrow.right.x, mFaceArray[i].leftEyebrow.right.y, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].leftEyebrow.top.x, mFaceArray[i].leftEyebrow.top.y, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].leftEyebrow.bottom.x, mFaceArray[i].leftEyebrow.bottom.y, 5f,
                            mouthBrush);

                    canvas.drawCircle(mFaceArray[i].rightEyebrow.left.x, mFaceArray[i].rightEyebrow.left.y, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].rightEyebrow.right.x, mFaceArray[i].rightEyebrow.right.y, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].rightEyebrow.top.x, mFaceArray[i].rightEyebrow.top.y, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].rightEyebrow.bottom.x, mFaceArray[i].rightEyebrow.bottom.y, 5f,
                            mouthBrush);

                    canvas.drawCircle(mFaceArray[i].leftEar.top.x * scaleX, mFaceArray[i].leftEar.top.y * scaleY, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].leftEar.bottom.x * scaleX, mFaceArray[i].leftEar.bottom.y * scaleY,
                            5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].rightEar.top.x * scaleX, mFaceArray[i].rightEar.top.y * scaleY, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].rightEar.bottom.x * scaleX, mFaceArray[i].rightEar.bottom.y
                            * scaleY, 5f, mouthBrush);

                    canvas.drawCircle(mFaceArray[i].leftEyeObj.left.x * scaleX, mFaceArray[i].leftEyeObj.left.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].leftEyeObj.right.x * scaleX, mFaceArray[i].leftEyeObj.right.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].leftEyeObj.top.x * scaleX, mFaceArray[i].leftEyeObj.top.y * scaleY,
                            5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].leftEyeObj.bottom.x * scaleX, mFaceArray[i].leftEyeObj.bottom.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].leftEyeObj.centerPupil.x * scaleX,
                            mFaceArray[i].leftEyeObj.centerPupil.y * scaleY, 5f, mouthBrush);

                    canvas.drawCircle(mFaceArray[i].rightEyeObj.left.x * scaleX, mFaceArray[i].rightEyeObj.left.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].rightEyeObj.right.x * scaleX, mFaceArray[i].rightEyeObj.right.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].rightEyeObj.top.x * scaleX, mFaceArray[i].rightEyeObj.top.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].rightEyeObj.bottom.x * scaleX, mFaceArray[i].rightEyeObj.bottom.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].rightEyeObj.centerPupil.x * scaleX,
                            mFaceArray[i].rightEyeObj.centerPupil.y * scaleY, 5f, mouthBrush);

                    canvas.drawCircle(mFaceArray[i].chin.left.x * scaleX, mFaceArray[i].chin.left.y * scaleY, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].chin.right.x * scaleX, mFaceArray[i].chin.right.y * scaleY, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].chin.center.x * scaleX, mFaceArray[i].chin.center.y * scaleY, 5f,
                            mouthBrush);

                    canvas.drawCircle(mFaceArray[i].nose.noseBridge.x * scaleX, mFaceArray[i].nose.noseBridge.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].nose.noseCenter.x * scaleX, mFaceArray[i].nose.noseCenter.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].nose.noseTip.x * scaleX, mFaceArray[i].nose.noseTip.y * scaleY, 5f,
                            mouthBrush);
                    canvas.drawCircle(mFaceArray[i].nose.noseLowerLeft.x * scaleX, mFaceArray[i].nose.noseLowerLeft.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].nose.noseLowerRight.x * scaleX, mFaceArray[i].nose.noseLowerRight.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].nose.noseMiddleLeft.x * scaleX, mFaceArray[i].nose.noseMiddleLeft.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].nose.noseMiddleRight.x * scaleX,
                            mFaceArray[i].nose.noseMiddleRight.y * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].nose.noseUpperLeft.x * scaleX, mFaceArray[i].nose.noseUpperLeft.y
                            * scaleY, 5f, mouthBrush);
                    canvas.drawCircle(mFaceArray[i].nose.noseUpperRight.x * scaleX, mFaceArray[i].nose.noseUpperRight.y
                            * scaleY, 5f, mouthBrush);
                }
                rectBrush.setColor(Color.YELLOW);
                rectBrush.setStyle(Paint.Style.STROKE);
                canvas.drawRect(mFaceArray[i].rect.left * scaleX, mFaceArray[i].rect.top * scaleY,
                        mFaceArray[i].rect.right * scaleX, mFaceArray[i].rect.bottom * scaleY, rectBrush);

            }
        } else {
            canvas.drawColor(0, Mode.CLEAR);
        }
    }
}
