#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/Checker.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Checker.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Wood.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Wood.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Marble.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Marble.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Clouds.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Clouds.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/ProceduralTexture.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Bunny.mesh assets/Samples/Meshes

chmod -R 777 assets
