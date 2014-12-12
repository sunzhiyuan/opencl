#!/bin/bash


mkdir -p src
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Kernels
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/Graph.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Graph.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Kernels/InteropCLGLES.cl assets/Samples/Kernels
cp ../../../../../Bin/Samples/Textures/InteropCLGLES.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts

chmod -R 777 assets
