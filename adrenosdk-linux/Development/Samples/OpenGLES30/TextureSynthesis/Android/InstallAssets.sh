#!/bin/bash

mkdir -p assets/Samples/Meshes
mkdir -p assets/Samples/Shaders
mkdir -p assets/Samples/Fonts
mkdir -p assets/Samples/Textures
mkdir -p assets/Samples/Misc/TextureSynthesis

cp ../../../../../Bin/Samples/Shaders/Overlay.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/Overlay.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/RenderTerrain.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/RenderTerrain.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ResampleHeightBaseColor.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ResampleHeightBaseColor.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ResampleHeightMapColor.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ResampleHeightMapColor.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ComputeNormals.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ComputeNormals.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/AddLayer.vs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/AddLayer.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Shaders/ComputeLayerLighting.fs assets/Samples/Shaders
cp ../../../../../Bin/Samples/Fonts/Tuffy12.pak assets/Samples/Fonts
cp ../../../../../Bin/Samples/Textures/TextureSynthesis.pak assets/Samples/Textures

cp ../../../../Assets/Samples/Misc/TextureSynthesis/demo1.srf assets/Samples/Misc/TextureSynthesis
cp ../../../../Assets/Samples/Misc/TextureSynthesis/demo2.srf assets/Samples/Misc/TextureSynthesis
cp ../../../../Assets/Samples/Misc/TextureSynthesis/rainier2049.raw assets/Samples/Misc/TextureSynthesis

chmod -R 777 assets
