#!/bin/bash

PAK_TOOL=Run_ResourcePacker.sh
SHADER_TOOL=ShaderPreprocessor
BIN_DIR=../../../../Bin

IN_DIR_BASE=../Development/Assets/Samples/Shaders/
IN_DIR_NPR=../Development/Assets/Samples/Shaders/NPR/
IN_DIR_POSTPROCESS=../Development/Assets/Samples/Shaders/PostProcessing/
IN_DIR_PROCEDURAL=../Development/Assets/Samples/Shaders/Procedural/

# == Demo paths == 
IN_DIR_TEXTURES_ADRENOSHADERS=../Development/Assets/Demos/AdrenoShaders/Textures/
IN_DIR_TEXTURES_ROBOT=../Development/Assets/Demos/Robot/Textures/pak/
IN_DIR_TEXTURES_DISINTEGRATE=../Development/Assets/Demos/Disintegrate/Textures/
				 
IN_DIR_FONTS_ADRENOSHADERS=../Development/Assets/Demos/AdrenoShaders/Fonts/
IN_DIR_FONTS_ROBOT=../Development/Assets/Demos/Robot/Fonts/
IN_DIR_FONTS_DISINTEGRATE=../Development/Assets/Demos/Disintegrate/Fonts/

OUT_DIR_ADRENOSHADERS_TEXTURES=./Demos/AdrenoShaders/Textures
OUT_DIR_ROBOT_TEXTURES=./Demos/Robot/Textures
OUT_DIR_DISINTEGRATE_TEXTURES=./Demos/Disintegrate/Textures

OUT_DIR_ADRENOSHADERS_FONT=./Demos/AdrenoShaders/Fonts
OUT_DIR_ROBOT_FONT=./Demos/Robot/Fonts
OUT_DIR_DISINTEGRATE_FONT=./Demos/Disintegrate/Fonts

IN_DIR_DEMO_FONTS=../Development/Assets/Demos/AdrenoShaders/Fonts/

mkdir -p $OUT_DIR_FONT
mkdir -p $OUT_DIR_TEXTURES
mkdir -p $OUT_SHADER_DIR


chmod +x $BIN_DIR/Run_ResourcePacker.sh

#Demo Meshes
cp -r -p -f ../../../Assets/Demos/AdrenoShaders/Meshes ../../../../Bin/Demos/AdrenoShaders/Meshes
cp -r -p -f ../../../Assets/Demos/Robot/Meshes ../../../../Bin/Demos/Robot/Meshes
cp -r -p -f ../../../Assets/Demos/Disintegrate/Meshes ../../../../Bin/Demos/Disintegrate/Meshes

cd $BIN_DIR
#Demo Fonts
./$PAK_TOOL $IN_DIR_FONTS_ADRENOSHADERS $OUT_DIR_ADRENOSHADERS_FONT/
./$PAK_TOOL $IN_DIR_FONTS_ROBOT $OUT_DIR_ROBOT_FONT/
./$PAK_TOOL $IN_DIR_FONTS_DISINTEGRATE $OUT_DIR_DISINTEGRATE_FONT/
#Demo Textures
./$PAK_TOOL $IN_DIR_TEXTURES_ADRENOSHADERS $OUT_DIR_ADRENOSHADERS_TEXTURES/
./$PAK_TOOL $IN_DIR_TEXTURES_ROBOT $OUT_DIR_ROBOT_TEXTURES/
./$PAK_TOOL $IN_DIR_TEXTURES_DISINTEGRATE $OUT_DIR_DISINTEGRATE_TEXTURES/
chmod 777 $IN_DIR_TEXTURES_DISINTEGRATE/noise_lum.tga
chmod 777 $IN_DIR_TEXTURES_ADRENOSHADERS/*.*
chmod -R 777 $OUT_DIR_ADRENOSHADERS_TEXTURES
chmod -R 777 $OUT_DIR_DISINTEGRATE_TEXTURES
cp $IN_DIR_TEXTURES_DISINTEGRATE/noise_lum.tga $OUT_DIR_DISINTEGRATE_TEXTURES/
cp $IN_DIR_TEXTURES_ADRENOSHADERS/*.tga $OUT_DIR_ADRENOSHADERS_TEXTURES/
cp $IN_DIR_TEXTURES_ADRENOSHADERS/*.png $OUT_DIR_ADRENOSHADERS_TEXTURES/

