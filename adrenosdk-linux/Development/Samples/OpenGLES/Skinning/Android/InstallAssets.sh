#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/Skinning.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Skinning.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/Skinning.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Boy03.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Boy03.anim assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Dman.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Dman.anim assets/Samples/Meshes

chmod -R 777 assets
