@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\Skinning.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Skinning.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\Skinning.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Boy03.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Boy03.anim assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Dman.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Dman.anim assets\Samples\Meshes

@echo Assets installed.

