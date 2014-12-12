#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/ParallaxMapping.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ParallaxMapping.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/ParallaxMapping.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Plane.mesh assets/Samples/Meshes

chmod -R 777 assets
