#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/MaterialShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/MaterialShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/DownsizeShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/DownsizeShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BlurShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BlurShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CombineShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CombineShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/DepthOfField30.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Map3.mesh assets/Samples/Meshes

chmod -R 777 assets
