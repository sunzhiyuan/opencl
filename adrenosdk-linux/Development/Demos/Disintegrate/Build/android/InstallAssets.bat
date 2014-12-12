@echo off

mkdir assets\Demos\Disintegrate

xcopy /Y /S ..\..\..\..\..\bin\Demos\Disintegrate\* assets\Demos\Disintegrate\*

@echo Assets installed.

