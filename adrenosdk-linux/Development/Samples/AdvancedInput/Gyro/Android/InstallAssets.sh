#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/DOFTexture.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/DOFTexture.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Meshes/BowlingPin.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Plane.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Textures/Gyro.pak assets/Samples/Textures


echo "Assets installed."

chmod -R 777 assets
