#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

# Shaders
cp ../../../../../Bin/Samples/Shaders/PhysicallyBasedLighting.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CookTorrance.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CookTorranceFull.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/KemenSzirmayKalos.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PhysicallyBasedLightingOptimized.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BlinnPhong.fs assets/Samples/Shaders

# Fonts
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts

# Texture PAK file (Textures)
cp ../../../../../Bin/Samples/Textures/PhysicallyBasedLighting.pak assets/Samples/Textures

# Meshes
cp ../../../../../Bin/Samples/Meshes/BumpySphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Cube.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Rocket.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Sphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Torus.mesh assets/Samples/Meshes

chmod -R 777 assets
