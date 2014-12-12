#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Blur.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Blur.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/NormalDeformer.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Constant.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Gaussian.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Gaussian.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Blend.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BlendAdditive.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Texture.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Texture.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/Bloom.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Teapot.mesh assets/Samples/Meshes

chmod -R 777 assets
