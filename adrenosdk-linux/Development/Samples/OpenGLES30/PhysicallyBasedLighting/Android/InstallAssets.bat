@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

:: Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PhysicallyBasedLighting.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CookTorrance.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CookTorranceFull.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\KemenSzirmayKalos.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PhysicallyBasedLightingOptimized.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BlinnPhong.fs assets\Samples\Shaders

:: PAK file for Fonts
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts

:: PAK file for Textures
copy ..\..\..\..\..\Bin\Samples\Textures\PhysicallyBasedLighting.pak assets\Samples\Textures

:: Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\BumpySphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Cube.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Rocket.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Sphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Torus.mesh assets\Samples\Meshes


@echo Assets installed.

