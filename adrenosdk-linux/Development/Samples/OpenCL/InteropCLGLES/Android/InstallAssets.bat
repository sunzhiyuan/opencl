@echo off

mkdir src
mkdir assets\Samples\Fonts
mkdir assets\Samples\Shaders
mkdir assets\Samples\Kernels
mkdir assets\Samples\Textures

copy ..\..\..\..\..\Bin\Samples\Shaders\Graph.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Graph.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Kernels\InteropCLGLES.cl assets\Samples\Kernels
copy ..\..\..\..\..\Bin\Samples\Textures\InteropCLGLES.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts

@echo Assets installed.

