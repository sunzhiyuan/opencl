#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/HDRTextureShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/MaterialShader9995Rev.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/HDRTextureShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/HDRTexture.pak assets/Samples/Textures
cp ../../../../../Development/Assets/Samples/Textures/HDRTexture1.tga assets/Samples/Textures
cp ../../../../../Development/Assets/Samples/Textures/HDRTexture2.tga assets/Samples/Textures

chmod -R 777 assets
