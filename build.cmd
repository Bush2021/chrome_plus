@echo off
setlocal EnableExtensions DisableDelayedExpansion

set "generator=ninja"
set "architecture=x64"
set "configuration=MinSizeRel"
set "mode=release"
set "build_version="

:parse
if "%~1"=="" goto parsed
set "option=%~1"
if /i "%option%"=="version" goto use_split_version
if /i "%option:~0,8%"=="version=" goto use_version
if /i "%~1"=="ninja" goto use_ninja
if /i "%~1"=="vs" goto use_vs
if /i "%~1"=="x86" goto use_x86
if /i "%~1"=="x64" goto use_x64
if /i "%~1"=="arm64" goto use_arm64
if /i "%~1"=="debug" goto use_debug
if /i "%~1"=="release" goto use_release
if /i "%~1"=="help" goto help
if /i "%~1"=="--help" goto help
if /i "%~1"=="-h" goto help
echo Unknown build option: %~1
echo.
goto help_error

:use_ninja
set "generator=ninja"
shift
goto parse

:use_vs
set "generator=vs"
shift
goto parse

:use_x86
set "architecture=x86"
shift
goto parse

:use_x64
set "architecture=x64"
shift
goto parse

:use_arm64
set "architecture=arm64"
shift
goto parse

:use_debug
set "configuration=Debug"
set "mode=debug"
shift
goto parse

:use_release
set "configuration=MinSizeRel"
set "mode=release"
shift
goto parse

:use_version
set "build_version=%option:~8%"
if not defined build_version (
  echo Build version cannot be empty.
  exit /b 1
)
shift
goto parse

:use_split_version
shift
if "%~1"=="" (
  echo Build version cannot be empty.
  exit /b 1
)
set "build_version=%~1"
shift
goto parse

:parsed
set "source_dir=%~dp0"
set "build_dir=%source_dir%build\%generator%-%architecture%"
set "artifact_dir=%build_dir%\%configuration%"
set "output_dir=%source_dir%out\%architecture%\%mode%"

set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" (
  echo Visual Studio Installer could not be found.
  echo Install Visual Studio with the Desktop development with C++ workload.
  exit /b 1
)

for /f "usebackq tokens=*" %%I in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "vs_dir=%%I"
if not defined vs_dir (
  echo Visual Studio C++ tools could not be found.
  echo Install the Desktop development with C++ workload.
  exit /b 1
)

where cmake.exe >nul 2>nul
if errorlevel 1 call :prepare_cmake
if errorlevel 1 exit /b %errorlevel%

if "%generator%"=="ninja" call :prepare_ninja
if errorlevel 1 exit /b %errorlevel%

if "%generator%"=="ninja" goto configure_ninja
goto configure_vs

:configure_ninja
cmake --preset ninja -B "%build_dir%" ^
  -DCHROME_PLUS_ARCH=%architecture% ^
  "-DCHROME_PLUS_BUILD_VERSION=%build_version%"
goto configured

:configure_vs
call :set_vs_arch
cmake --preset vs -B "%build_dir%" -A %vs_arch% ^
  -DCHROME_PLUS_ARCH=%architecture% ^
  "-DCHROME_PLUS_BUILD_VERSION=%build_version%"

:configured
if errorlevel 1 exit /b %errorlevel%

cmake --build "%build_dir%" --config %configuration% --parallel
if errorlevel 1 exit /b %errorlevel%

cmake -E make_directory "%output_dir%"
if errorlevel 1 exit /b %errorlevel%
cmake -E copy_if_different ^
  "%artifact_dir%\version.dll" ^
  "%source_dir%src\chrome++.ini" ^
  "%output_dir%"
if errorlevel 1 exit /b %errorlevel%
if exist "%artifact_dir%\version.pdb" (
  cmake -E copy_if_different ^
    "%artifact_dir%\version.pdb" ^
    "%output_dir%\version.pdb"
  if errorlevel 1 exit /b %errorlevel%
)

echo.
echo Build output: %output_dir%
exit /b 0

:prepare_cmake
set "cmake_dir=%vs_dir%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin"
if exist "%cmake_dir%\cmake.exe" goto cmake_found
echo CMake could not be found.
echo Install CMake tools for Windows in Visual Studio Installer.
exit /b 1

:cmake_found
set "PATH=%cmake_dir%;%PATH%"
exit /b 0

:prepare_ninja
if "%architecture%"=="x86" set "vcvars_arch=amd64_x86"
if "%architecture%"=="x64" set "vcvars_arch=amd64"
if "%architecture%"=="arm64" set "vcvars_arch=amd64_arm64"

call "%vs_dir%\VC\Auxiliary\Build\vcvarsall.bat" %vcvars_arch%
if errorlevel 1 exit /b %errorlevel%

where ninja.exe >nul 2>nul
if errorlevel 1 call :prepare_ninja_path
if errorlevel 1 exit /b %errorlevel%

where clang-cl.exe >nul 2>nul
if errorlevel 1 call :prepare_clang_path
if errorlevel 1 exit /b %errorlevel%

where lld-link.exe >nul 2>nul
if errorlevel 1 (
  echo LLD could not be found.
  echo Install the Clang tools for Windows component.
  exit /b 1
)

where llvm-rc.exe >nul 2>nul
if errorlevel 1 (
  echo LLVM Resource Compiler could not be found.
  echo Install the Clang tools for Windows component.
  exit /b 1
)
exit /b 0

:prepare_ninja_path
set "ninja_dir=%vs_dir%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
if exist "%ninja_dir%\ninja.exe" goto ninja_found
echo Ninja could not be found.
echo Install CMake tools for Windows in Visual Studio Installer.
exit /b 1

:ninja_found
set "PATH=%ninja_dir%;%PATH%"
exit /b 0

:prepare_clang_path
if exist "%vs_dir%\VC\Tools\Llvm\x64\bin\clang-cl.exe" goto clang_x64
if exist "%vs_dir%\VC\Tools\Llvm\bin\clang-cl.exe" goto clang_default
echo Clang-CL could not be found.
echo Install the Clang tools for Windows component.
exit /b 1

:clang_x64
set "PATH=%vs_dir%\VC\Tools\Llvm\x64\bin;%PATH%"
exit /b 0

:clang_default
set "PATH=%vs_dir%\VC\Tools\Llvm\bin;%PATH%"
exit /b 0

:set_vs_arch
if "%architecture%"=="x86" set "vs_arch=Win32"
if "%architecture%"=="x64" set "vs_arch=x64"
if "%architecture%"=="arm64" set "vs_arch=ARM64"
exit /b 0

:help
echo Usage: build [ninja^|vs] [x86^|x64^|arm64] [debug^|release] [version=VALUE]
echo.
echo Defaults: ninja x64 release
echo.
echo Examples:
echo   build
echo   build debug
echo   build arm64 release
echo   build vs
echo   build vs x86 debug
echo   build version=1.19.0
exit /b 0

:help_error
echo Usage: build [ninja^|vs] [x86^|x64^|arm64] [debug^|release] [version=VALUE]
exit /b 1
