setlocal enabledelayedexpansion

echo off

echo %CD%
mkdir ..\..\..\..\Bin\Samples\Meshes\
::.obj
::..\..\..\Assets\Samples\Meshes\Source\
for %%i in (..\..\..\Assets\Samples\Meshes\Source\*.obj) do (

set inputFilePath=%%i
::echo %%i
set outputFilePath="..\..\..\..\Bin\Samples\Meshes\%%~ni.mesh"
echo !outputFilePath!
..\..\..\..\Bin\MeshConverter -nologo %%i -o !outputFilePath!
)



::

::echo %outputFilePath%
::echo %@outputFilePath%
::echo off
::echo %%i
::echo %%~ni