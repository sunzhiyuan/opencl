#!/bin/bash


mkdir -p src
mkdir -p assets/Samples/Kernels
mkdir -p assets/Samples/Textures

cp ../../../../../Bin/Samples/Kernels/ImageMedianFilter.cl assets/Samples/Kernels
cp ../../../../../Bin/Samples/Textures/SampleImageUncompressed.tga assets/Samples/Textures

chmod -R 777 assets
