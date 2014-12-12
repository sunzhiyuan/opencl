#!/bin/bash

BIN_DIR="../../../../../../$1"
TARGET_DIR="../../../../../../$1/$2.app/Contents/MacOS"

mkdir -p ${BIN_DIR}/Tutorials
mkdir -p ${BIN_DIR}/Tutorials/OpenGLES
mkdir -p ${TARGET_DIR}/Tutorials
mkdir -p ${TARGET_DIR}/Tutorials/OpenGLES

chmod 777 ../../../../../Assets/Tutorials/OpenGLES/TreeBark.tga
cp ../../../../../Assets/Tutorials/OpenGLES/TreeBark.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/TreeBark.tga ${TARGET_DIR}/Tutorials/OpenGLES
