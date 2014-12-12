@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\PointSprites.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PointSprites.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\PointSprites.pak assets\Samples\Textures

@echo Assets installed.

