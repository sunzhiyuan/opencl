#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/Texture.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Texture.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Noise.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Noise.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Blur.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Blur.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Sharpen.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Sharpen.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Static.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Static.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Sepia.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Sepia.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Sobel.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Sobel.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Emboss.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Emboss.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Lens.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Lens.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Warp.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Warp.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/ImageEffects.pak assets/Samples/Textures

echo "assets installed."

chmod -R 777 assets
