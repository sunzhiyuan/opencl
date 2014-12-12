@echo off

mkdir src
mkdir assets\Samples\Kernels
mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures

copy ..\..\..\..\..\Bin\Samples\Shaders\PointSprites.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PointSprites.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\ParticleSystemCLGLES.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Kernels\ParticleSystemCLGLES.cl assets\Samples\Kernels

@echo Assets installed.

