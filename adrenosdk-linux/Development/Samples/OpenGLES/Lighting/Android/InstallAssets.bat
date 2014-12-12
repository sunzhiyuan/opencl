@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PerVertexLighting.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PerVertexLighting.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\Lighting.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Phone.mesh assets\Samples\Meshes

@echo Assets installed.

