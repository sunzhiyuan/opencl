#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/CelShading30.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CelShading30.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Blend30.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BlendMultiplicative30.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/NPR30.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/RayGun.mesh assets/Samples/Meshes

chmod -R 777 assets
