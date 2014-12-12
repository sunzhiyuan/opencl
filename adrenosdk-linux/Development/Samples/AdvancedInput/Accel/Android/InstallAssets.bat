@echo off

@echo.
@echo ------------------------------------------------------------------------------
@echo This will copy the required data into the assets directory for sample to run.
@echo ------------------------------------------------------------------------------
@echo.

pause

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\DOFTexture.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\DOFTexture.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Meshes\BowlingPin.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Plane.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Textures\Accel.pak assets\Samples\Textures

@echo.
@echo DONE
pause

