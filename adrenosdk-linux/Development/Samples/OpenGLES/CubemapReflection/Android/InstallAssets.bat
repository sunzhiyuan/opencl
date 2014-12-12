@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\CubemapReflectionMain.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CubemapReflectionMain.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CubemapReflectionSatellite.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CubemapReflectionSatellite.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\CubemapReflection.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Trophy.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\TennisBall.mesh assets\Samples\Meshes

@echo Assets installed.

