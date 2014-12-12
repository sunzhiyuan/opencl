#!/bin/bash

SHADER_TOOL=ShaderPreprocessor
BIN_DIR=../../../../Bin
OUT_DIR=$BIN_DIR/Samples/Shaders/

IN_DIR_BASE=$BIN_DIR/../Development/Assets/Samples/Shaders/
IN_DIR_NPR=$BIN_DIR/../Development/Assets/Samples/Shaders/NPR/
IN_DIR_POSTPROCESS=$BIN_DIR/../Development/Assets/Samples/Shaders/PostProcessing/
IN_DIR_PROCEDURAL=$BIN_DIR/../Development/Assets/Samples/Shaders/Procedural/

$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_BASE -out $OUT_DIR
$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_NPR -out $OUT_DIR
$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_POSTPROCESS -out $OUT_DIR
$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_PROCEDURAL -out $OUT_DIR


