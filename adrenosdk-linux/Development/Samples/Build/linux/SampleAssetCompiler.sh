#!/bin/bash

PAK_TOOL=Run_ResourcePacker.sh
SHADER_TOOL=ShaderPreprocessor
BIN_DIR=../../../../Bin
OUT_SHADER_DIR=/Samples/Shaders/

IN_DIR_FONT=../Development/Assets/Samples/Fonts/
OUT_DIR_FONT=./Samples/Fonts

IN_DIR_TEXTURES=../Development/Assets/Samples/Textures/
OUT_DIR_TEXTURES=./Samples/Textures

IN_DIR_BASE=../Development/Assets/Samples/Shaders/
IN_DIR_NPR=../Development/Assets/Samples/Shaders/NPR/
IN_DIR_POSTPROCESS=../Development/Assets/Samples/Shaders/PostProcessing/
IN_DIR_PROCEDURAL=../Development/Assets/Samples/Shaders/Procedural/

mkdir -p $OUT_DIR_FONT
mkdir -p $OUT_DIR_TEXTURES
mkdir -p $OUT_SHADER_DIR


chmod +x $BIN_DIR/Run_ResourcePacker.sh

cd $BIN_DIR
#export LD_PRELOAD=$BIN_DIR/libfreeimage.so 
./$PAK_TOOL $IN_DIR_FONT $OUT_DIR_FONT/
./$PAK_TOOL $IN_DIR_TEXTURES $OUT_DIR_TEXTURES/
#IN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_BASE -out $OUT_SHADER_DIR
#$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_NPR -out $OUT_SHADER_DIR
#$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_POSTPROCESS -out $OUT_SHADER_DIR
#$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_PROCEDURAL -out $OUT_SHADER_DIR

# xcopy /Y /S ..\..\..\..\Assets\Samples\Meshes\* $(TargetDir)\Samples\Meshes\* > NUL
