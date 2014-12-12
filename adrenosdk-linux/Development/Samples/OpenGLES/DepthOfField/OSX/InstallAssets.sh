#!/bin/bash

#BIN_DIR=../../../../../$1
BIN_DIR=../../../../BinaryAssets
TARGET_DIR="${BIN_DIR}/$2.app/Contents/MacOS/Samples"

mkdir -p ${TARGET_DIR}/Meshes
mkdir -p ${TARGET_DIR}/Shaders
mkdir -p ${TARGET_DIR}/Fonts
mkdir -p ${TARGET_DIR}/Textures

cp ${BIN_DIR}/Samples/Shaders/DOF.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/DOF.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/DOFTexture.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/DOFTexture.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Gaussian.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Gaussian.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
cp ${BIN_DIR}/Samples/Textures/DepthOfField.pak ${TARGET_DIR}/Textures
cp ${BIN_DIR}/Samples/Meshes/BowlingPin.mesh ${TARGET_DIR}/Meshes

