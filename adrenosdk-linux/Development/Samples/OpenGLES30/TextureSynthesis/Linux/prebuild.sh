#!/bin/bash

SRC_DIR=../../../../Assets/Samples/Misc/TextureSynthesis
TARGET_DIR=../../../../../Bin/Samples/Misc/TextureSynthesis

if [ "$1" == "clean" ]; then
	rm -rf $TARGET_DIR
else
	mkdir -p $TARGET_DIR
	cp -R $SRC_DIR/* $TARGET_DIR
fi




