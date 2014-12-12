@echo off

mkdir assets\Samples\Shaders
mkdir assets\Samples\Fonts
mkdir assets\Samples\Textures
mkdir assets\Samples\Meshes
mkdir assets\Samples\Misc\TextureSynthesis

copy ..\..\..\..\..\Bin\Samples\Shaders\Overlay.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\Overlay.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\RenderTerrain.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\RenderTerrain.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ResampleHeightBaseColor.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ResampleHeightBaseColor.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ResampleHeightMapColor.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ResampleHeightMapColor.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ComputeNormals.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ComputeNormals.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\AddLayer.vs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\AddLayer.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Shaders\ComputeLayerLighting.fs assets\Samples\Shaders
copy ..\..\..\..\..\Bin\Samples\Fonts\Tuffy12.pak assets\Samples\Fonts
copy ..\..\..\..\..\Bin\Samples\Textures\TextureSynthesis.pak assets\Samples\Textures

copy ..\..\..\..\Assets\Samples\Misc\TextureSynthesis\demo1.srf assets\Samples\Misc\TextureSynthesis
copy ..\..\..\..\Assets\Samples\Misc\TextureSynthesis\demo2.srf assets\Samples\Misc\TextureSynthesis
copy ..\..\..\..\Assets\Samples\Misc\TextureSynthesis\rainier2049.raw assets\Samples\Misc\TextureSynthesis


@echo Assets installed.

