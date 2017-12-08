@echo off

setlocal

set "CarNDKidnappedVehicleProjectPlatform=x86"
set "CarNDKidnappedVehicleProjectToolset=v140"
set "CarNDKidnappedVehicleProjectBuildType=Debug"

if NOT "%~1"=="" set "CarNDKidnappedVehicleProjectPlatform=%~1"
if NOT "%~2"=="" set "CarNDKidnappedVehicleProjectToolset=%~2"
if NOT "%~3"=="" set "CarNDKidnappedVehicleProjectBuildType=%~3" 

set "VcPkgDir=c:\vcpkg"
set "VcPkgTriplet=%CarNDKidnappedVehicleProjectPlatform%-windows"
rem set "VcPkgTriplet=%CarNDKidnappedVehicleProjectPlatform%-windows-%CarNDKidnappedVehicleProjectToolset%"

if defined VCPKG_ROOT_DIR if /i not "%VCPKG_ROOT_DIR%"=="" (
    set "VcPkgDir=%VCPKG_ROOT_DIR%"
)
if defined VCPKG_DEFAULT_TRIPLET if /i not "%VCPKG_DEFAULT_TRIPLET%"=="" (
    set "VcpkgTriplet=%VCPKG_DEFAULT_TRIPLET%"
)
set "VcPkgPath=%VcPkgDir%\vcpkg.exe"

echo. & echo Bootstrapping dependencies for triplet: %VcPkgTriplet% & echo.

rem ==============================
rem Update and Install packages
rem ==============================
call "%VcPkgPath%" update

rem Install latest uwebsockets
call "%VcPkgPath%" install uwebsockets --triplet %VcPkgTriplet%
rem Use adapted main.cpp for latest uwebsockets
copy main.cpp ..\..\src

rem ==============================
rem Configure CMake
rem ==============================

set "VcPkgTripletDir=%VcPkgDir%\installed\%VcPkgTriplet%"

set "CMAKE_PREFIX_PATH=%VcPkgTripletDir%;%CMAKE_PREFIX_PATH%"

echo. & echo Bootstrapping successful for triplet: %VcPkgTriplet% & echo CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% & echo.

set "CarNDKidnappedVehicleProjectCMakeGeneratorName=Visual Studio 15 2017"

if "%CarNDKidnappedVehicleProjectPlatform%"=="x86" (
    if "%CarNDKidnappedVehicleProjectToolset%"=="v140" set "CarNDKidnappedVehicleProjectCMakeGeneratorName=Visual Studio 14 2015"
    if "%CarNDKidnappedVehicleProjectToolset%"=="v141" set "CarNDKidnappedVehicleProjectCMakeGeneratorName=Visual Studio 15 2017"
)

if "%CarNDKidnappedVehicleProjectPlatform%"=="x64" (
    if "%CarNDKidnappedVehicleProjectToolset%"=="v140" set "CarNDKidnappedVehicleProjectCMakeGeneratorName=Visual Studio 14 2015 Win64"
    if "%CarNDKidnappedVehicleProjectToolset%"=="v141" set "CarNDKidnappedVehicleProjectCMakeGeneratorName=Visual Studio 15 2017 Win64"
)

set "CarNDKidnappedVehicleProjectBuildDir=%~dp0\..\..\products\cmake.msbuild.windows.%CarNDKidnappedVehicleProjectPlatform%.%CarNDKidnappedVehicleProjectToolset%"
if not exist "%CarNDKidnappedVehicleProjectBuildDir%" mkdir "%CarNDKidnappedVehicleProjectBuildDir%"
cd "%CarNDKidnappedVehicleProjectBuildDir%"

echo: & echo CarNDKidnappedVehicleProjectBuildDir=%CD% & echo cmake.exe -G "%CarNDKidnappedVehicleProjectCMakeGeneratorName%" "%~dp0\..\.." & echo:

call cmake.exe -G "%CarNDKidnappedVehicleProjectCMakeGeneratorName%" "%~dp0\..\.."

call "%VcPkgPath%" integrate install

endlocal