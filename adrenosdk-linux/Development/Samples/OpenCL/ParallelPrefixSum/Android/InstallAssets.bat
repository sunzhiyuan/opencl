@echo off

mkdir src
mkdir assets\Samples\Kernels

copy ..\..\..\..\..\Bin\Samples\Kernels\ParallelPrefixSum.cl assets\Samples\Kernels

@echo Assets installed.

