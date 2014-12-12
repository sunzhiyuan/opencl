#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/SWE.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/SWE.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/SWE.pak assets/Samples/Textures

chmod -R 777 assets
