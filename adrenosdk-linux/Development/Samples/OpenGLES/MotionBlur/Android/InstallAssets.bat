@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\Texture.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\TextureAlpha.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Constant.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Constant.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\MotionBlur.pak assets\Samples\Textures

@echo Assets installed.

