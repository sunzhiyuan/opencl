@echo off

mkdir src
mkdir assets\Samples\Kernels
mkdir assets\Samples\Textures

copy ..\..\..\..\..\Bin\Samples\Kernels\ImageHistogram.cl assets\Samples\Kernels
copy ..\..\..\..\..\Bin\Samples\Textures\SampleImageUncompressed.tga assets\Samples\Textures

@echo Assets installed.

