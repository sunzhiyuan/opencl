@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\NormalMapping.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\NormalMapping.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\NoNormalMapping.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\NoNormalMapping.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\NormalMapping.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Map3.mesh assets\Samples\Meshes

@echo Assets installed.

