#!/bin/bash

SRC_DIR=../../../../Assets/Samples/Textures
TARGET_DIR=../../../../../Bin/Samples/Textures

if [ "$1" == "clean" ]; then
	rm -f $TARGET_DIR/HDRTexture1.tga
	rm -f $TARGET_DIR/HDRTexture2.tga
else
	mkdir -p $TARGET_DIR
	cp $SRC_DIR/HDRTexture1.tga $TARGET_DIR
	cp $SRC_DIR/HDRTexture2.tga $TARGET_DIR
fi





