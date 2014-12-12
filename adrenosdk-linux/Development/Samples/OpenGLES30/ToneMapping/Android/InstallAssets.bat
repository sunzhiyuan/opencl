@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes

copy ..\..\..\..\..\Bin\Samples\Shaders\MaterialShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\MaterialShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\DownsizeShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\DownsizeShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\GenerateLuminance.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\GenerateLuminance.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\AverageLuminance.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\AverageLuminance.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\AdaptLuminance.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\AdaptLuminance.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BrightPass.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BrightPass.fs assets\Samples\Shaders
copy "..\..\..\..\..\Bin\Samples\Shaders\Copy.vs" assets\Samples\Shaders
copy "..\..\..\..\..\Bin\Samples\Shaders\Copy.fs" assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BlurShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\BlurShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CombineShaderToneMapping.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\CombineShaderToneMapping.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PreviewShader.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\PreviewShader.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\ToneMapping.pak assets\Samples\Textures
copy ..\..\..\..\..\Bin\Samples\Meshes\Map3.mesh assets\Samples\Meshes

@echo Assets installed.

