#!/bin/bash

#BIN_DIR=../../../../../$1
BIN_DIR=../../../../BinaryAssets
TARGET_DIR="${BIN_DIR}/$2.app/Contents/MacOS/Samples"

mkdir -p ${TARGET_DIR}/Meshes
mkdir -p ${TARGET_DIR}/Shaders
mkdir -p ${TARGET_DIR}/Fonts
mkdir -p ${TARGET_DIR}/Textures

cp ${BIN_DIR}/Samples/Shaders/CelShading.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/CelShading.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/NormalDepth.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/NormalDepth.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Sobel.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Sobel.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Texture.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Texture.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Blend.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/BlendMultiplicative.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
cp ${BIN_DIR}/Samples/Textures/NPR.pak ${TARGET_DIR}/Textures
cp ${BIN_DIR}/Samples/Meshes/RayGun.mesh ${TARGET_DIR}/Meshes
