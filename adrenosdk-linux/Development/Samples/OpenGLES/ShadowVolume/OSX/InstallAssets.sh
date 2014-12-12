#!/bin/bash

BIN_DIR=../../../../BinaryAssets
#BIN_DIR=../../../../../$1
TARGET_DIR="${BIN_DIR}/$2.app/Contents/MacOS/Samples"

mkdir -p ${TARGET_DIR}/Meshes
mkdir -p ${TARGET_DIR}/Shaders
mkdir -p ${TARGET_DIR}/Fonts
mkdir -p ${TARGET_DIR}/Textures

cp ${BIN_DIR}/Samples/Shaders/ShadowVolume.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/ShadowVolume.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/PerPixelLighting.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/PerPixelLighting.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Constant.vs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Shaders/Constant.fs ${TARGET_DIR}/Shaders
cp ${BIN_DIR}/Samples/Fonts/Tuffy12.pak ${TARGET_DIR}/Fonts
cp ${BIN_DIR}/Samples/Textures/ShadowVolume.pak ${TARGET_DIR}/Textures
cp ${BIN_DIR}/Samples/Meshes/Plane.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/PlaneEdges.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Terrain.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/TerrainEdges.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Cube.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/CubeEdges.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Sphere.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/SphereEdges.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/BumpySphere.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/BumpySphereEdges.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Torus.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/TorusEdges.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/Rocket.mesh ${TARGET_DIR}/Meshes
cp ${BIN_DIR}/Samples/Meshes/RocketEdges.mesh ${TARGET_DIR}/Meshes
