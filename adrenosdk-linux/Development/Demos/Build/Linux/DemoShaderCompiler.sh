#!/bin/bash

SHADER_TOOL=ShaderPreprocessor
BIN_DIR=../../../../Bin


IN_DIR_ADRENOSHADERS=$BIN_DIR/../Development/Assets/Demos/AdrenoShaders/Shaders/
OUT_DIR_ADRENOSHADERS=$BIN_DIR/Demos/AdrenoShaders/Shaders/

IN_DIR_ROBOT=$BIN_DIR/../Development/Assets/Demos/Robot/Shaders/
OUT_DIR_ROBOT=$BIN_DIR/Demos/Robot/Shaders/

IN_DIR_DISINTEGRATE=$BIN_DIR/../Development/Assets/Demos/Disintegrate/Shaders/
OUT_DIR_DISINTEGRATE=$BIN_DIR/Demos/Disintegrate/Shaders/

# Demo shaders
$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_ADRENOSHADERS -out $OUT_DIR_ADRENOSHADERS
$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_ROBOT -out $OUT_DIR_ROBOT
$BIN_DIR/$SHADER_TOOL -nologo -in $IN_DIR_DISINTEGRATE -out $OUT_DIR_DISINTEGRATE
