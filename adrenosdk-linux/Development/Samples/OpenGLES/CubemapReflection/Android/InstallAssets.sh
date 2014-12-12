#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/CubemapReflectionMain.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CubemapReflectionMain.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CubemapReflectionSatellite.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CubemapReflectionSatellite.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/CubeMapReflection.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/trophy.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/tennisBall.mesh assets/Samples/Meshes

chmod -R 777 assets
