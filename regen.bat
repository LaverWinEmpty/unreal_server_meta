@echo off
setlocal enabledelayedexpansion

:: engine path
set "ue_path=D:\ue5.4.3"

:: project path
set "uproject_path=%~dp0Meta.uproject"

:: keep list
set "keep_files=Meta.uproject .gitignore regen.bat Meta.sln"

:: keep list
set "keep_folders=Source Content Config ThirdParty Build"

:: directory
set "folder=%~dp0"

:: del files
for %%F in ("%folder%\*") do (
    set "target_name=%%~nxF"
    echo !keep_files! | findstr /I /C:"!target_name!" >nul || del /F /Q "%%F"
)

:: del folders
for /D %%D in ("%folder%\*") do (
    set "target_name=%%~nxD"
    echo !keep_folders! | findstr /I /C:"!target_name!" >nul || rd /S /Q "%%D"
)

:: regen solution
"%ue_path%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -ProjectFiles -Project="%uproject_path%" -Game -Engine