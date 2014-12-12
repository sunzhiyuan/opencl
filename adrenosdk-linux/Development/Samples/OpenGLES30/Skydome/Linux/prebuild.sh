#!/bin/bash

SRC_DIR=../../../../Assets/Samples/Textures
TARGET_DIR=../../../../../Bin/Samples/Textures

if [ "$1" == "clean" ]; then
	rm -f $TARGET_DIR/heights.tga
	rm -f $TARGET_DIR/normals.tga
	rm -f $TARGET_DIR/terrain.tga
else
	mkdir -p $TARGET_DIR
	cp $SRC_DIR/heights.tga $TARGET_DIR
	cp $SRC_DIR/normals.tga $TARGET_DIR
	cp $SRC_DIR/terrain.tga $TARGET_DIR
fi




