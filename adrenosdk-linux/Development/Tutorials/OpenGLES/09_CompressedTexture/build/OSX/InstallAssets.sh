#!/bin/bash

BIN_DIR="../../../../../../$1"
TARGET_DIR="../../../../../../$1/$2.app/Contents/MacOS"

mkdir -p ${BIN_DIR}/Tutorials
mkdir -p ${BIN_DIR}/Tutorials/OpenGLES
mkdir -p ${TARGET_DIR}/Tutorials
mkdir -p ${TARGET_DIR}/Tutorials/OpenGLES

chmod 777 ../../../../../Assets/Tutorials/OpenGLES/RainbowFilm.atc
cp ../../../../../Assets/Tutorials/OpenGLES/RainbowFilm.atc ${BIN_DIR}/Tutorials/OpenGLES
cp ../../../../../Assets/Tutorials/OpenGLES/RainbowFilm.atc ${TARGET_DIR}/Tutorials/OpenGLES
