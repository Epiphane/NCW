@echo off

SETLOCAL

set LIB_VERSION=09072018
set BUILD_DIR=%LIB_VERSION%\build

set BUILDHOST=X64

REM Windows SDK defines
REM ---------------------------------------------------------------------
set WINKIT_PATH=C:\Program Files (x86)\Windows Kits\10
set WINKIT_VERSION=10.0.17763.0
set WINKITINCLUDE=%WINKIT_PATH%/Include/%WINKIT_VERSION%
set INCLUDE=%WINKITINCLUDE%/ucrt;%WINKITINCLUDE%/um;%WINKITINCLUDE%/shared;

REM Visual Studio defines
REM ---------------------------------------------------------------------
set VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
set VS_VERSION=14.20.27508
set INCLUDE=%INCLUDE%;%VS_PATH%/VC/Tools/MSVC/%VS_VERSION%/include

CALL :Compile x64
CALL :Compile x86

set OPTS=
EXIT /B %ERRORLEVEL%

:Compile
set ARCH=%~1
rmdir /S /Q .\%BUILD_DIR%\%ARCH%
mkdir .\%BUILD_DIR%\%ARCH%

echo ----------------
echo Compiling %ARCH%
"%VS_PATH%\VC\Tools\MSVC\%VS_VERSION%\bin\Host%ARCH%\%ARCH%\CL.exe" ^
   /c ^
   /ZI /JMC /nologo /W3 /WX- ^
   /diagnostics:classic /sdl /Od /Oy- /D _MBCS /Gm- /EHsc /RTC1 /MDd /GS ^
   /fp:precise /permissive- /Zc:wchar_t /Zc:forScope /Zc:inline ^
   /Fo"%BUILD_DIR%\%ARCH%\\" ^
   /Fd"%BUILD_DIR%\%ARCH%\glad.pdb" ^
   /Gd /TC /analyze- /FC /errorReport:prompt ^
   /I%LIB_VERSION%\include ^
   %LIB_VERSION%\src\glad.c
echo Linking %ARCH%.lib
"%VS_PATH%\VC\Tools\MSVC\%VS_VERSION%\bin\Host%ARCH%\%ARCH%\Lib.exe" ^
   /OUT:"%BUILD_DIR%\%ARCH%\glad.lib" ^
   /NOLOGO /MACHINE:%ARCH% ^
   "%BUILD_DIR%\%ARCH%\glad.obj"
echo Linking %ARCH%.dll
"%VS_PATH%\VC\Tools\MSVC\%VS_VERSION%\bin\Host%ARCH%\%ARCH%\Lib.exe" ^
   /OUT:"%BUILD_DIR%\%ARCH%\glad.dll" ^
   /NOLOGO /MACHINE:%ARCH% ^
   "%BUILD_DIR%\%ARCH%\glad.obj"

EXIT /B 0
