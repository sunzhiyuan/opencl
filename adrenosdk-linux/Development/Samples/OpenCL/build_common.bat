@echo off
rem build the OpenCL Common library

rem push the current directory on the stack, so we can get back to it
pushd %CD%

rem go to the common folder
cd Common
call ant clean
call android update project -p . -n Common -t android-19
call ant debug

popd

mkdir BandwidthTest\Android\src
mkdir ClothSimCLGLES\Android\src
mkdir Concurrency\Android\src
mkdir DeviceQuery\Android\src
mkdir DotProduct\Android\src
mkdir FFT1D\Android\src
mkdir FFT2D\Android\src
mkdir ImageBoxFilter\Android\src
mkdir ImageMedianFilter\Android\src
mkdir ImageRecursiveGaussianFilter\Android\src
mkdir ImageSobelFilter\Android\src
mkdir InteropCLGLES\Android\src
mkdir MatrixMatrixMul\Android\src
mkdir MatrixTranspose\Android\src
mkdir MatrixVectorMul\Android\src
mkdir ParallelPrefixSum\Android\src
mkdir ParticleSystemCLGLES\Android\src
mkdir PostProcessCLGLES\Android\src
mkdir VectorAdd\Android\src
