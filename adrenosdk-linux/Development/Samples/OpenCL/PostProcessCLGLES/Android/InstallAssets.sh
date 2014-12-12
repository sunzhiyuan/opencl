#!/bin/bash


mkdir -p src
mkdir -p assets/Samples/Kernels
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures
mkdir -p assets/Samples/Meshes

cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Texture.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Texture.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/PostProcessCLGLES.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Kernels/PostProcessCLGLES.cl assets/Samples/Kernels
cp ../../../../../Bin/Samples/Meshes/Phone.mesh assets/Samples/Meshes

chmod -R 777 assets
