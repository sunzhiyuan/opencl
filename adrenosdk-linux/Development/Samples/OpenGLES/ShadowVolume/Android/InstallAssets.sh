#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/ShadowVolume.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ShadowVolume.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Constant.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Constant.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/ShadowVolume.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Plane.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/PlaneEdges.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Terrain.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/TerrainEdges.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Cube.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/CubeEdges.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Sphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/SphereEdges.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/BumpySphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/BumpySphereEdges.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Torus.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/TorusEdges.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Rocket.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/RocketEdges.mesh assets/Samples/Meshes

chmod -R 777 assets
