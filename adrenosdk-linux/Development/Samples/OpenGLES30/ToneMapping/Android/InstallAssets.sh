#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/MaterialShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/MaterialShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/DownsizeShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/DownsizeShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/GenerateLuminance.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/GenerateLuminance.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/AverageLuminance.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/AverageLuminance.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/AdaptLuminance.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/AdaptLuminance.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BrightPass.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BrightPass.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Copy.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Copy.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BlurShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/BlurShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CombineShaderToneMapping.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CombineShaderToneMapping.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PreviewShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PreviewShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/ToneMapping.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Map3.mesh assets/Samples/Meshes

chmod -R 777 assets
