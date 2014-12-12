@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\Texture.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Texture.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Noise.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Noise.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Blur.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Blur.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Sharpen.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Sharpen.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Static.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Static.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Sepia.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Sepia.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Sobel.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Sobel.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Emboss.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Emboss.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Lens.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Lens.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Warp.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Warp.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\ImageEffects.pak assets\Samples\Textures

@echo Assets installed.

