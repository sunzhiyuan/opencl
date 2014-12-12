#!/bin/bash

BIN_DIR="../../../../../../$1"
TARGET_DIR="../../../../../../$1/$2.app/Contents/MacOS"

mkdir -p ${BIN_DIR}/Tutorials
mkdir -p ${BIN_DIR}/Tutorials/OpenGLES
mkdir -p ${BIN_DIR}/Tutorials/OpenGLES/12_CubemapTexture
mkdir -p ${TARGET_DIR}/Tutorials
mkdir -p ${TARGET_DIR}/Tutorials/OpenGLES
mkdir -p ${TARGET_DIR}/Tutorials/OpenGLES/12_CubemapTexture

chmod 777 ../../../../../Assets/Tutorials/OpenGLES/12_CubemapTexture/*.tga
cp ../../../../../Assets/Tutorials/OpenGLES/12_CubemapTexture/*.tga ${BIN_DIR}/Tutorials/OpenGLES/12_CubemapTexture
cp ../../../../../Assets/Tutorials/OpenGLES/12_CubemapTexture/*.tga ${TARGET_DIR}/Tutorials/OpenGLES/12_CubemapTexture
