@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\ShadowVolume.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ShadowVolume.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Constant.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Constant.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\ShadowVolume.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Plane.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Terrain.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Cube.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Sphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\BumpySphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Torus.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Rocket.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\PlaneEdges.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\TerrainEdges.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\CubeEdges.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\SphereEdges.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\BumpySphereEdges.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\TorusEdges.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\RocketEdges.mesh assets\Samples\Meshes

@echo Assets installed.

