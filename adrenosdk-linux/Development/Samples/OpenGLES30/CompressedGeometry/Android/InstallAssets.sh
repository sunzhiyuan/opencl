#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/CompressedGeometryF16.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CompressedGeometryF16.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CompressedGeometry101012.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CompressedGeometry101012.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CompressedGeometry24.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CompressedGeometry24.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CompressedGeometry8.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CompressedGeometry8.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/CompressedGeometry.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Cube.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Sphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/BumpySphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Torus.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Rocket.mesh assets/Samples/Meshes

chmod -R 777 assets
