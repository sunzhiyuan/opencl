#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PerPixelLighting.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PerVertexLighting.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PerVertexLighting.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/Lighting.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Phone.mesh assets/Samples/Meshes

echo "assets installed."

chmod -R 777 assets
