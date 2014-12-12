#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Shaders/Overlay.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Overlay.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/OverlayArrayFirstElement.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/OverlayArrayFirstElement.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PreviewShader.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PreviewShader.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Depth.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Depth.fs assets/Samples/Shaders

cp ../../../../../Bin/Samples/Shaders/CSM.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Hard.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PCFShowSplits.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/PCF.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/HardArray.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/CSMOptimized.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/OptimizedExp.fs assets/Samples/Shaders

cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/CascadedShadowMaps.pak assets/Samples/Textures
cp ../../../../../Bin/Samples/Meshes/Cube.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Sphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/BumpySphere.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Torus.mesh assets/Samples/Meshes
cp ../../../../../Bin/Samples/Meshes/Rocket.mesh assets/Samples/Meshes

chmod -R 777 assets
