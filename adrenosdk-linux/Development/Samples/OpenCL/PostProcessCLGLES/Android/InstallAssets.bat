@echo off

mkdir src
mkdir assets\Samples\Kernels
mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Texture.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Texture.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\PostProcessCLGLES.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Kernels\PostProcessCLGLES.cl assets\Samples\Kernels
copy ..\..\..\..\..\Bin\Samples\Meshes\Phone.mesh assets\Samples\Meshes

@echo Assets installed.

