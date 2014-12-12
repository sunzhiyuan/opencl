#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/CelShading.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CelShading.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/NormalDepth.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/NormalDepth.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Sobel.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Sobel.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Texture.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Texture.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Blend.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BlendMultiplicative.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/NPR.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/RayGun.mesh assets/Samples/Meshes

chmod -R 777 assets
