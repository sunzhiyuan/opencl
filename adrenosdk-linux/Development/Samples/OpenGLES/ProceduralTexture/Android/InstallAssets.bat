@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\Checker.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Checker.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Wood.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Wood.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Marble.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Marble.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Clouds.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Clouds.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\ProceduralTexture.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Bunny.mesh assets\Samples\Meshes

@echo Assets installed.

