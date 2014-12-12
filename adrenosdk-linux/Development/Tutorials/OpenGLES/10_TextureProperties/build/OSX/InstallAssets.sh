#!/bin/bash

BIN_DIR="../../../../../../$1"
TARGET_DIR="../../../../../../$1/$2.app/Contents/MacOS"

mkdir -p ${BIN_DIR}/Tutorials
mkdir -p ${BIN_DIR}/Tutorials/OpenGLES
mkdir -p ${TARGET_DIR}/Tutorials
mkdir -p ${TARGET_DIR}/Tutorials/OpenGLES

chmod 777 ../../../../../Assets/Tutorials/OpenGLES/Texture*.tga
cp ../../../../../Assets/Tutorials/OpenGLES/Texture1.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture1d.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture2.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture3.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture4.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture5.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture6.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture7.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture8.tga ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture1.tga ${TARGET_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture1d.tga ${TARGET_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture2.tga ${TARGET_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture3.tga ${TARGET_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture4.tga ${TARGET_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture5.tga ${TARGET_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture6.tga ${TARGET_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture7.tga ${TARGET_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/Texture8.tga ${TARGET_DIR}/Tutorials/OpenGLES
