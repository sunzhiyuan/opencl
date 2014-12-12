#!/bin/bash

#BIN_DIR=../../../../../$1
BIN_DIR=../../../../BinaryAssets
TARGET_DIR="${BIN_DIR}/$2.app/Contents/MacOS/Samples"


mkdir -p ${TARGET_DIR}/Meshes
mkdir -p ${TARGET_DIR}/Shaders
mkdir -p ${TARGET_DIR}/Fonts
mkdir -p ${TARGET_DIR}/Textures


cp ${BIN_DIR}/Samples/Shaders/PerPixelLighting.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/PerPixelLighting.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Blur.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Blur.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/NormalDeformer.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Constant.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Gaussian.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Gaussian.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Blend.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/BlendAdditive.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Texture.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Texture.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
cp ${BIN_DIR}/Samples/Textures/Bloom.pak ${TARGET_DIR}/Textures
cp ${BIN_DIR}/Samples/Meshes/Teapot.mesh ${TARGET_DIR}/Meshes
