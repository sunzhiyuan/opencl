adb remount
adb shell mkdir -p        /data/local/opencl/cl_demo_tests/wavelet_transform 
adb push dwt_forward_x.cl /data/local/opencl/cl_demo_tests/wavelet_transform 
adb push dwt_forward_y.cl /data/local/opencl/cl_demo_tests/wavelet_transform 
adb push dwt_inverse_x.cl /data/local/opencl/cl_demo_tests/wavelet_transform 
adb push dwt_inverse_y.cl /data/local/opencl/cl_demo_tests/wavelet_transform 
adb push haar_forward.cl  /data/local/opencl/cl_demo_tests/wavelet_transform 
adb push haar_inverse.cl  /data/local/opencl/cl_demo_tests/wavelet_transform 
adb push test.ycbcr /data/local/opencl/cl_demo_tests/wavelet_transform  
adb push cl_demo_test_waveletransform /data/local/opencl/cl_demo_tests/wavelet_transform

adb shell "cd /data/local/dwttransform/ && ls -l"
