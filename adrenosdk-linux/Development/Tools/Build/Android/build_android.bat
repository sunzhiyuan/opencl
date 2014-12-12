@echo off
rem build and install the sample apk

rem push the current directory on the stack, so we can get back to it
pushd %CD%

rem get to the sample jni, build the .so
cd ../../TextureConverter/ASTCLib/build/Android/jni
call ndk-build  

popd
pushd %CD%
cd ../../TextureConverter/build/android/jni
call ndk-build  

popd

exit /B 1

@echo on
