@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\Skinning2.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Skinning2.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\Skinning2.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\scorpion.model assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\scorpion.anim assets\Samples\Meshes

@echo Assets installed.

