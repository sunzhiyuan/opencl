#!/bin/bash

# Couldn't find "goto" statement for bash (like the batch script equivalent of this file has)
# Need to find a way to report/handle build failure for "InstallAssets" and "ant debug" 

curDir=${PWD}	# Store the current working directory..

cd ../..

if [ -e "install_android.sh" ]
then
chmod 777 install_android.sh
else
    echo ../../install_android.sh script does not exist
    exit
fi 

./install_android.sh 01_CreateWindow
./install_android.sh 02_ConfigureFrameBuffer
./install_android.sh 03_DrawTriangle
./install_android.sh 04_PortraitAndLandscape
./install_android.sh 05_NonInterleavedVBO
./install_android.sh 06_InterleavedVBO
./install_android.sh 07_Transforms
./install_android.sh 08_SimpleTexture
./install_android.sh 09_CompressedTexture
./install_android.sh 10_TextureProperties
./install_android.sh 11_MultiTexture
./install_android.sh 12_CubemapTexture
./install_android.sh 13_CompressedVertices
./install_android.sh 14_StencilBuffer
./install_android.sh 15_RenderToTexture
./install_android.sh 16_RenderToTextureOGLES30
./install_android.sh 17_FBOTextureFormatsOGLES30
./install_android.sh 18_InterleavedVBOOGLES30
./install_android.sh 19_InterleavedVBOOGLES30
./install_android.sh 20_Texture2DArrayOGLES30
./install_android.sh 21_PixelBufferObjectsOGLES30
./install_android.sh 22_TransformFeedbackOGLES30
./install_android.sh 23_BlitFramebufferOGLES30
./install_android.sh 24_MultisampleFramebufferOGLES30
./install_android.sh 25_FrameBufferTextureLayerOGLES30

cd ${curDir}
