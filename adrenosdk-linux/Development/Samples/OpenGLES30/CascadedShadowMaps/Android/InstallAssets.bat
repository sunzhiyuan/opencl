@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\Overlay.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Overlay.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\OverlayArrayFirstElement.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\OverlayArrayFirstElement.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PreviewShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PreviewShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Depth.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Depth.fs assets\Samples\Shaders

copy ..\..\..\..\..\Bin\Samples\Shaders\CSM.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Hard.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PCFShowSplits.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PCF.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\HardArray.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CSMOptimized.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\OptimizedExp.fs assets\Samples\Shaders

copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\CascadedShadowMaps.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Cube.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Sphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\BumpySphere.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Torus.mesh assets\Samples\Meshes
copy ..\..\..\..\..\Bin\Samples\Meshes\Rocket.mesh assets\Samples\Meshes


@echo Assets installed.

