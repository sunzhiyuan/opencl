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
cp ${BIN_DIR}/Samples/Shaders/Constant.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Constant.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
cp ${BIN_DIR}/Samples/Textures/ProjectiveShadows.pak ${TARGET_DIR}/Textures
cp ${BIN_DIR}/Samples/Meshes/Plane.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Cube.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Sphere.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/BumpySphere.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Torus.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Rocket.mesh ${TARGET_DIR}/Meshes
