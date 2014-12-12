@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\HDRTextureShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\MaterialShader9995Rev.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\HDRTextureShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\HDRTexture.pak assets\Samples\Textures
copy ..\..\..\..\..\Development\Assets\Samples\Textures\HDRTexture1.tga assets\Samples\Textures
copy ..\..\..\..\..\Development\Assets\Samples\Textures\HDRTexture2.tga assets\Samples\Textures


@echo Assets installed.

