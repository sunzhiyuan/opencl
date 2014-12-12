@echo off
rem build and install the sample apk

rem push the current directory on the stack, so we can get back to it
pushd %CD%

IF "%1" == "" goto errorNoSampleName

rem get to the sample jni, build the .so
cd %1/Build/android/jni
call %ANDROID_NDK%/ndk-build && (echo success) || (goto errorBuildFail)

rem back up to android, build the apk
cd ..
call InstallAssets && (echo success) || (goto errorBuildFail)
call android update project -p . -t android-14
call ant debug && (echo success) || (goto errorBuildFail)

IF "%2" NEQ "install" goto noInstall

call ant installd

:noInstall
popd
goto end

:errorNoSampleName
echo Error. No sample name defined or not executing from Sample root. 
echo Usage: build_android SampleName
goto end

:errorBuildFail
echo Error: the sample failed to build.
popd
exit /B 1

:end
@echo on
