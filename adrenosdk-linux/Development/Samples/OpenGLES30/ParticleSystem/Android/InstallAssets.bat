@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\PointSprites30.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PointSprites30.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\EmitParticles30.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\EmitParticles30.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\ParticleSystem.pak assets\Samples\Textures


@echo Assets installed.

