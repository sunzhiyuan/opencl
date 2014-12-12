mkdir ..\..\..\..\Bin_d
xcopy "..\..\..\..\Bin\Demos\*.*" /Y /S  "..\..\..\..\Bin_d\Demos\*.*" > NUL
echo Demo assets copied.
xcopy "..\..\..\..\Bin\Samples\*.*" /Y /S "..\..\..\..\Bin_d\Samples\*.*" > NUL
echo Sample assets copied.
xcopy "..\..\..\..\Bin\Tutorials\*.*" /Y /S "..\..\..\..\Bin_d\Tutorials\*.*" > NUL
echo Tutorial assets copied.
xcopy "..\..\..\..\Bin\ShaderPreprocessor.exe" /Y /S "..\..\..\..\Bin_d\"
xcopy "..\..\..\..\Bin\ResourcePacker.exe" /Y /S "..\..\..\..\Bin_d\"
echo Utilities copied.
echo Done.