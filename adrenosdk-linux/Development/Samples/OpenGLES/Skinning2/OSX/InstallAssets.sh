#!/bin/bash

BIN_DIR=../../../../BinaryAssets
#BIN_DIR=../../../../../$1
TARGET_DIR="${BIN_DIR}/$2.app/Contents/MacOS/Samples"

mkdir -p ${TARGET_DIR}/Meshes
mkdir -p ${TARGET_DIR}/Shaders
mkdir -p ${TARGET_DIR}/Fonts
mkdir -p ${TARGET_DIR}/Textures

cp ${BIN_DIR}/Samples/Shaders/Skinning2.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Skinning2.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
cp ${BIN_DIR}/Samples/Textures/Skinning2.pak ${TARGET_DIR}/Textures
cp ${BIN_DIR}/Samples/Meshes/scorpion.model ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/scorpion.anim ${TARGET_DIR}/Meshes
