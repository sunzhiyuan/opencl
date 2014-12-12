#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/HoffmanScatter.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/HoffmanScatter.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/HoffmanScatterTerrain.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/SkyDome.pak assets/Samples/Textures

cp ../../../../../Development/Assets/Samples/Textures/heights.tga assets/Samples/Textures
cp ../../../../../Development/Assets/Samples/Textures/normals.tga assets/Samples/Textures
cp ../../../../../Development/Assets/Samples/Textures/terrain.tga assets/Samples/Textures

chmod -R 777 assets
