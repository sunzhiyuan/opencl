@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\ParallaxMapping.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ParallaxMapping.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\ParallaxMapping.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Plane.mesh assets\Samples\Meshes

@echo Assets installed.

