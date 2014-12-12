@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\MaterialShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\MaterialShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\DownsizeShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\DownsizeShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BlurShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BlurShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CombineShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CombineShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\DepthOfField30.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Map3.mesh assets\Samples\Meshes


@echo Assets installed.

