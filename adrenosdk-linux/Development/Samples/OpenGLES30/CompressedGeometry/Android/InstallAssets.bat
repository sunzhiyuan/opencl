@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\CompressedGeometryF16.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CompressedGeometryF16.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CompressedGeometry101012.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CompressedGeometry101012.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CompressedGeometry24.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CompressedGeometry24.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CompressedGeometry8.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CompressedGeometry8.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\CompressedGeometry.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Cube.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Sphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\BumpySphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Torus.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Rocket.mesh assets\Samples\Meshes

@echo Assets installed.

