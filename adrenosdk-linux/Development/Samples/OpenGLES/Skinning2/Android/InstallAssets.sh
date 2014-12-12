#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/Skinning2.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Skinning2.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/Skinning2.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/scorpion.model assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/scorpion.anim assets/Samples/Meshes

chmod -R 777 assets
