#!/bin/bash

BIN_DIR=../../../../BinaryAssets
#BIN_DIR=../../../../../$1
TARGET_DIR="${BIN_DIR}/$2.app/Contents/MacOS/Samples"

mkdir -p ${TARGET_DIR}/Meshes
mkdir -p ${TARGET_DIR}/Shaders
mkdir -p ${TARGET_DIR}/Fonts
mkdir -p ${TARGET_DIR}/Textures

cp ${BIN_DIR}/Samples/Shaders/ShadowMap.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/ShadowMap.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/ShadowMap2x2PCF.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/PerPixelLighting.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/PerPixelLighting.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Overlay.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Overlay.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Depth.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Depth.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
cp ${BIN_DIR}/Samples/Textures/ShadowMap.pak ${TARGET_DIR}/Textures
cp ${BIN_DIR}/Samples/Meshes/Plane.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Terrain.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Cube.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Sphere.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/BumpySphere.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Torus.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Rocket.mesh ${TARGET_DIR}/Meshes
