
@echo off
rem build and install the sample apk

rem push the current directory on the stack, so we can get back to it
pushd %CD%

cd ../..

call install_android 01_CreateWindow || (goto errorBuildFail)
call install_android 02_ConfigureFrameBuffer || (goto errorBuildFail)
call install_android 03_DrawTriangle || (goto errorBuildFail)
call install_android 04_PortraitAndLandscape || (goto errorBuildFail)
call install_android 05_NonInterleavedVBO || (goto errorBuildFail)
call install_android 06_InterleavedVBO || (goto errorBuildFail)
call install_android 07_Transforms || (goto errorBuildFail)
call install_android 08_SimpleTexture || (goto errorBuildFail)
call install_android 09_CompressedTexture || (goto errorBuildFail)
call install_android 10_TextureProperties || (goto errorBuildFail)
call install_android 11_MultiTexture || (goto errorBuildFail)
call install_android 12_CubemapTexture || (goto errorBuildFail)
call install_android 13_CompressedVertices || (goto errorBuildFail)
call install_android 14_StencilBuffer || (goto errorBuildFail)
call install_android 15_RenderToTexture || (goto errorBuildFail)
call install_android 16_RenderToTextureOGLES30 || (goto errorBuildFail)
call install_android 17_FBOTextureFormatsOGLES30 || (goto errorBuildFail)
call install_android 18_InterleavedVBOOGLES30 || (goto errorBuildFail)
call install_android 19_InterleavedVBOOGLES30 || (goto errorBuildFail)
call install_android 20_Texture2DArrayOGLES30 || (goto errorBuildFail)
call install_android 21_PixelBufferObjectsOGLES30 || (goto errorBuildFail)
call install_android 22_TransformFeedbackOGLES30 || (goto errorBuildFail)
call install_android 23_BlitFramebufferOGLES30 || (goto errorBuildFail)
call install_android 24_MultisampleFramebufferOGLES30 || (goto errorBuildFail)
call install_android 25_FrameBufferTextureLayerOGLES30 || (goto errorBuildFail)
goto end

:errorBuildFail
echo Error: the sample failed to build.
popd
exit /B 1


:end
popd

@echo on
