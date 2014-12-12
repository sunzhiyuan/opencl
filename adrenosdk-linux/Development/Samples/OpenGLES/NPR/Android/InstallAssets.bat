@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\CelShading.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CelShading.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\NormalDepth.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\NormalDepth.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Sobel.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Sobel.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Texture.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Texture.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Blend.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BlendMultiplicative.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\NPR.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\RayGun.mesh assets\Samples\Meshes

@echo Assets installed.

