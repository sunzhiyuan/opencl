/*======================================================================
*  FILE:
*      readme.txt
*  DESCRIPTION:
*      readme for wavelet transform application   
*
*            Copyright (c) 2012 QUALCOMM Incorporated.
*                      All Rights Reserved.
*              QUALCOMM Confidential and Proprietary
*
*======================================================================*/

This folder contains a sample code that does Haar transform and wavelet transform. 

For android: 
1. To build android, please run Android.mk. Make sure the library files are in place. 
2. To run under android, run adbpush.sh first, then run run_tests.sh

For Windows:

1. You may use cmaker 2.6/2.8 to build a visual studio solution file. Or you can simply create a windows console project and add the .h and .cpp to your project. 

2. To run the code, for GPU device, run "WaveletTransform test.ycbcr test_out.ycbcr 768 512 4 GPU", for CPU device, run "WaveletTransform test.ycbcr test_out.ycbcr 768 512 4 CPU",  assuming the image dimension is 768x512 and format is 444 interleaved. 

Note the kernel files (.cl) are assumed to be under current folder where the executable file is located. 

