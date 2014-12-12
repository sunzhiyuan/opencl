#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/NormalMapping.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/NormalMapping.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/NoNormalMapping.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/NoNormalMapping.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/NormalMapping.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Map3.mesh assets/Samples/Meshes

chmod -R 777 assets
