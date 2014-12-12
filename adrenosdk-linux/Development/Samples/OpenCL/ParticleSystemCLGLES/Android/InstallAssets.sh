#!/bin/bash


mkdir -p src
mkdir -p assets/Samples/Kernels
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/PointSprites.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PointSprites.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/ParticleSystemCLGLES.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Kernels/ParticleSystemCLGLES.cl assets/Samples/Kernels

chmod -R 777 assets
