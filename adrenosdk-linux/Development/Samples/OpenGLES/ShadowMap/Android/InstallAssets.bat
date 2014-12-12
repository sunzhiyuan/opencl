@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\ShadowMap.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ShadowMap.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ShadowMap2x2PCF.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Overlay.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Overlay.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Depth.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Depth.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\ShadowMap.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Plane.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Terrain.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Cube.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Sphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\BumpySphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Torus.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Rocket.mesh assets\Samples\Meshes

@echo Assets installed.

