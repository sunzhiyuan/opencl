@echo off

mkdir assets\Demos\Robot

xcopy /Y /S ..\..\..\..\..\bin\Demos\Robot\* assets\Demos\Robot\*

@echo Assets installed.

