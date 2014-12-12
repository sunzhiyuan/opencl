@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\CelShading30.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CelShading30.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Blend30.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BlendMultiplicative30.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\NPR30.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\RayGun.mesh assets\Samples\Meshes


@echo Assets installed.

