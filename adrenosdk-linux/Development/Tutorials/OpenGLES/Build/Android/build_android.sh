#!/bin/bash

# Couldn't find "goto" statement for bash (like the batch script equivalent of this file has)
# Need to find a way to report/handle build failure for "InstallAssets" and "ant debug" 

curDir=${PWD}	# Store the current working directory..

cd ../..

if [ -e "build_android.sh" ]
then
chmod 777 build_android.sh
else
    echo ../../build_android.sh script does not exist
    exit
fi 

./build_android.sh 01_CreateWindow
./build_android.sh 02_ConfigureFrameBuffer
./build_android.sh 03_DrawTriangle
./build_android.sh 04_PortraitAndLandscape
./build_android.sh 05_NonInterleavedVBO
./build_android.sh 06_InterleavedVBO
./build_android.sh 07_Transforms
./build_android.sh 08_SimpleTexture
./build_android.sh 09_CompressedTexture
./build_android.sh 10_TextureProperties
./build_android.sh 11_MultiTexture
./build_android.sh 12_CubemapTexture
./build_android.sh 13_CompressedVertices
./build_android.sh 14_StencilBuffer
./build_android.sh 15_RenderToTexture
./build_android.sh 16_RenderToTextureOGLES30
./build_android.sh 17_FBOTextureFormatsOGLES30
./build_android.sh 18_InterleavedVBOOGLES30
./build_android.sh 19_InterleavedVBOOGLES30
./build_android.sh 20_Texture2DArrayOGLES30
./build_android.sh 21_PixelBufferObjectsOGLES30
./build_android.sh 22_TransformFeedbackOGLES30
./build_android.sh 23_BlitFramebufferOGLES30
./build_android.sh 24_MultisampleFramebufferOGLES30
./build_android.sh 25_FrameBufferTextureLayerOGLES30

cd ${curDir}
