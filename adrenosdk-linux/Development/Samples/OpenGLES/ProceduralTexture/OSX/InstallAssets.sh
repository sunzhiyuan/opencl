#!/bin/bash

BIN_DIR=../../../../BinaryAssets
#BIN_DIR=../../../../../$1
TARGET_DIR="${BIN_DIR}/$2.app/Contents/MacOS/Samples"

mkdir -p ${TARGET_DIR}/Meshes
mkdir -p ${TARGET_DIR}/Shaders
mkdir -p ${TARGET_DIR}/Fonts
mkdir -p ${TARGET_DIR}/Textures

cp ${BIN_DIR}/Samples/Shaders/Checker.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Checker.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Wood.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Wood.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Marble.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Marble.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Clouds.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Clouds.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
cp ${BIN_DIR}/Samples/Textures/ProceduralTexture.pak ${TARGET_DIR}/Textures
cp ${BIN_DIR}/Samples/Meshes/Bunny.mesh ${TARGET_DIR}/Meshes
