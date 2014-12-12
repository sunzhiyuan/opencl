#!/bin/bash

#BIN_DIR=../../../../../$1
BIN_DIR=../../../../BinaryAssets
TARGET_DIR="${BIN_DIR}/$2.app/Contents/MacOS/Samples"

mkdir -p ${TARGET_DIR}/Meshes
mkdir -p ${TARGET_DIR}/Shaders
mkdir -p ${TARGET_DIR}/Fonts
mkdir -p ${TARGET_DIR}/Textures

cp ${BIN_DIR}/Samples/Shaders/Texture.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Texture.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Noise.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Noise.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Blur.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Blur.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Sharpen.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Sharpen.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Static.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Static.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Sepia.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Sepia.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Sobel.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Sobel.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Emboss.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Emboss.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Lens.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Lens.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Warp.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Warp.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
#cp ${BIN_DIR}/Samples/Textures/ImageEffects.pak ${TARGET_DIR}/Textures
