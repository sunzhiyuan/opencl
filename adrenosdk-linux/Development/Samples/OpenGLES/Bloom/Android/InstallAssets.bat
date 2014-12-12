@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PerPixelLighting.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Blur.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Blur.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\NormalDeformer.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Constant.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Gaussian.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Gaussian.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Blend.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BlendAdditive.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Texture.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Texture.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\Bloom.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Teapot.mesh assets\Samples\Meshes

@echo Assets installed.

