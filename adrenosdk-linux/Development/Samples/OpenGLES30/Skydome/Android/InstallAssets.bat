@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\HoffmanScatter.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\HoffmanScatter.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\HoffmanScatterTerrain.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\SkyDome.pak assets\Samples\Textures

copy ..\..\..\..\Assets\Samples\Textures\heights.tga assets\Samples\Textures
copy ..\..\..\..\Assets\Samples\Textures\normals.tga assets\Samples\Textures
copy ..\..\..\..\Assets\Samples\Textures\terrain.tga assets\Samples\Textures



@echo Assets installed.

