#!/bin/bash

#BIN_DIR=../../../../../$1
BIN_DIR=../../../../BinaryAssets
TARGET_DIR="${BIN_DIR}/$2.app/Contents/MacOS/Samples"

mkdir -p ${TARGET_DIR}/Meshes
mkdir -p ${TARGET_DIR}/Shaders
mkdir -p ${TARGET_DIR}/Fonts
mkdir -p ${TARGET_DIR}/Textures

cp ${BIN_DIR}/Samples/Shaders/Skinning.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Skinning.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
cp ${BIN_DIR}/Samples/Textures/Skinning.pak ${TARGET_DIR}/Textures
cp ${BIN_DIR}/Samples/Meshes/Boy03.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Boy03.anim ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Dman.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Dman.anim ${TARGET_DIR}/Meshes
