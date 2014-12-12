#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/DOF.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/DOF.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/DOFTexture.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/DOFTexture.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Gaussian.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Gaussian.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/DepthOfField.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/BowlingPin.mesh assets/Samples/Meshes


echo "assets installed."

chmod -R 777 assets
