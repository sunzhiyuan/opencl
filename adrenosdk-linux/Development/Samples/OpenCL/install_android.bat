@echo off
rem build and install the sample apk

setlocal enabledelayedexpansion

set upload=yes

rem need to change target version number if using different version of andorid sdk
set target=android-19

IF "%1" == "all" (
set upload=no
goto :BUILD_ALL
)

IF "%1" == "" goto :errorNoSampleName
IF "%1" == "usage" goto :errorNoSampleName
IF "%1" == "apk" goto :INSTALL_ALL

set sample_name=%1
goto :BUILD_SAMPLE

:BUILD_ALL
set sample[0]=Common
set sample[1]=ClothSimCLGLES
set sample[2]=Concurrency
set sample[3]=DeviceQuery
set sample[4]=DotProduct
set sample[5]=FFT1D
set sample[6]=FFT2D
set sample[7]=ImageBoxFilter
set sample[8]=ImageMedianFilter
set sample[9]=ImageRecursiveGaussianFilter
set sample[10]=ImageSobelFilter
set sample[11]=InteropCLGLES
set sample[12]=MatrixMatrixMul
set sample[13]=MatrixTranspose
set sample[14]=MatrixVectorMul
set sample[15]=ParallelPrefixSum
set sample[16]=ParticleSystemCLGLES
set sample[17]=PostProcessCLGLES
set sample[18]=VectorAdd
set sample[19]=AdrenoOpenCLLauncher
set sample[20]=BandwidthTest

	   
for /l %%n in (0,1,20) do (
set sample_name=!sample[%%n]!
call:BUILD_SAMPLE
)
goto:eof

:INSTALL_ALL
for %%c in (bin\*.apk) do call adb install -r %%c   
goto:eof

:BUILD_SAMPLE
echo ############### Build %sample_name% ################
pushd %CD%
cd %sample_name%
set bin_path=bin

if "%sample_name%" == "AdrenoOpenCLLauncher" goto:PACKAGING
if "%sample_name%" == "Common" goto:PACKAGING

cd Android\jni
echo ################ clean ################
call ndk-build clean
echo ################ build ################
call ndk-build
cd ..
call InstallAssets.bat
set bin_path=Android\bin

:PACKAGING
echo ################ update build.xml ################
call android update project -p . -n %sample_name% -t %target%
echo ################ clean package ################
call ant clean
echo ################ packaging ################
call ant debug

popd

if "%1"=="Common" goto:eof

SET APK_FILE=%sample_name%\%bin_path%\%sample_name%-debug.apk
mkdir apk
copy /y %APK_FILE% apk\%sample_name%-debug.apk

if "%upload%" == "yes" (
echo install %sample_name%
call adb install -r apk\\%sample_name%-debug.apk
)
goto:eof

:errorNoSampleName
echo Usage: install_android all 
echo        ----Build every sample and copy apk to apk folder. Does not install to device
echo.
echo Usage: install_android [sample_name] 
echo        ----Build sample_name and install to device
echo.
echo Usage: install_android apk
echo        ----Install all *.apk from apk folder to device
goto end

:end
@echo on
