#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/ShadowMap.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ShadowMap.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ShadowMap2x2PCF.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Overlay.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Overlay.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Depth.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Depth.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/ShadowMap.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Plane.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Terrain.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Cube.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Sphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/BumpySphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Torus.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Rocket.mesh assets/Samples/Meshes

chmod -R 777 assets
