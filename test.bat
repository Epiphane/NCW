@echo off

REM "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\bin\Hostx64\x64\cl.exe" ^
REM "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\bin\Hostx64\x64\cl.exe" ^
REM    /Z7 /nologo /c /Zc:inline /Zc:strictStrings /fp:fast /diagnostics:caret /EHsc ^
REM    /Wall /WX /wd4625 /wd4626 /wd4668 /wd4774 /wd5026 /wd5027 /wd4061 /wd4201 /wd4255 /wd4464 /wd4514 /wd4571 /wd4577 /wd4710 /wd4711 /wd4820 /wd5039 /wd5045 ^
REM    /DDEBUG /D_DEBUG ^
REM    /I"./" /std:c++17 -I"C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.13.26128/include/" ^
REM    "C:\Users\Thomas\Code\CubeWorldC++\Unity1.cpp" ^
REM    /Fo"C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\RGBBinding\Unity1.obj" "-IC:\Program Files (x86)\Windows Kits\10/Include/10.0.17763.0/ucrt" "-IC:\Program Files (x86)\Windows Kits\10/Include/10.0.17763.0/um" "-IC:\Program Files (x86)\Windows Kits\10/Include/10.0.17763.0/shared" -DWIN32_LEAN_AND_MEAN -DWIN32 -D_WIN32 -D__WINDOWS__ -DWIN64 -DDEBUG -DPROFILING_ENABLED /MTd /Od /RTC1 /GS /Oy- /GR- /analyze -I"dependencies\rapidjson/1.1.0" -I"dependencies\glm/0.9.9.0" -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING -D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING -I"source/Library"  /showIncludes

   REM "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\RGBBinding\Unity1.cpp" ^

REM "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\bin\Hostx64\x64\cl.exe" /Z7 /nologo /c /Zc:inline /Zc:strictStrings /fp:fast /diagnostics:caret /EHsc /Wall /WX /wd4625 /wd4626 /wd4668 /wd4774 /wd5026 /wd5027 /wd28301 /wd4061 /wd4068 /wd4201 /wd4255 /wd4371 /wd4464 /wd4514 /wd4555 /wd4571 /wd4577 /wd4710 /wd4711 /wd4820 /wd4868 /wd5039 /wd5045 /I"./" /std:c++17 -I"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.20.27508/include/" ^
REM    "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\sqlite\Unity1.c"      ^
REM    /Fo"C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\sqlite\Unity1.obj"  "-IC:\Program Files (x86)\Windows Kits\10/Include/10.0.17763.0/ucrt" "-IC:\Program Files (x86)\Windows Kits\10/Include/10.0.17763.0/um" "-IC:\Program Files (x86)\Windows Kits\10/Include/10.0.17763.0/shared" -DWIN32_LEAN_AND_MEAN -DWIN32 -D_WIN32 -D__WINDOWS__ -DWIN64 -DDEBUG -DPROFILING_ENABLED -I"source/Library" /std:c++17 -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING -D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING -DNOMINMAX -DCUBEWORLD_PLATFORM_WINDOWS -D_WIN32_WINNT=0x0601 /MTd /Od /RTC1 /GS /Oy- /GR- /analyze -I"dependencies\sqlite/3.22.0" ^
REM    /wd4005 /wd4242 /wd4996 /wd6001 /wd6011 /wd6031 /wd6239 /wd6240 /wd6308 /wd6313 /wd6326 /wd6330 /wd6385 /wd6386 /wd6387 /wd28182 /wd28251  ^
REM    -I"dependencies\sqlite/3.22.0" /wd4005 /wd4242 /wd4996 /wd6001 /wd6011 /wd6031 /wd6239 /wd6240 /wd6308 /wd6313 /wd6326 /wd6330 /wd6385 /wd6386 /wd6387 /wd28182 /wd28251  ^
REM    /showIncludes

REM "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\bin\Hostx64\x64\cl.exe" /Z7 /nologo /c /Zc:inline /Zc:strictStrings /fp:fast /diagnostics:caret /EHsc /Wall /WX /wd4625 /wd4626 /wd4668 /wd4774 /wd5026 /wd5027 /wd28301 /wd4061 /wd4068 /wd4201 /wd4255 /wd4371 /wd4464 /wd4514 /wd4555 /wd4571 /wd4577 /wd4710 /wd4711 /wd4820 /wd4868 /wd5039 /wd5045 /I"./" /std:c++17 -I"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.20.27508/include/" ^
REM    "C:\Users\Thomas\Code\CubeWorldC++\dependencies\sqlite\3.22.0\shell.c" ^
REM    /Fo"C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\sqlite\shell.obj" "-IC:\Program Files (x86)\Windows Kits\10/Include/10.0.17763.0/ucrt" "-IC:\Program Files (x86)\Windows Kits\10/Include/10.0.17763.0/um" "-IC:\Program Files (x86)\Windows Kits\10/Include/10.0.17763.0/shared" -DWIN32_LEAN_AND_MEAN -DWIN32 -D_WIN32 -D__WINDOWS__ -DWIN64 -DDEBUG -DPROFILING_ENABLED -I"source/Library" /std:c++17 -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING -D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING -DNOMINMAX -DCUBEWORLD_PLATFORM_WINDOWS -D_WIN32_WINNT=0x0601 /MTd /Od /RTC1 /GS /Oy- /GR- /analyze -I"dependencies\sqlite/3.22.0" ^
REM    /wd4005 /wd4242 /wd4996 /wd6001 /wd6011 /wd6031 /wd6239 /wd6240 /wd6308 /wd6313 /wd6326 /wd6330 /wd6385 /wd6386 /wd6387 /wd28182 /wd28251 ^
REM    -I"dependencies\sqlite/3.22.0" /wd4005 /wd4242 /wd4996 /wd6001 /wd6011 /wd6031 /wd6239 /wd6240 /wd6308 /wd6313 /wd6326 /wd6330 /wd6385 /wd6386 /wd6387 /wd28182 /wd28251  ^
REM    /showIncludes


REM "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\bin\Hostx64\x64\link.exe" ^
REM    /NODEFAULTLIB ^
REM    /NOLOGO /INCREMENTAL:NO ^
REM    /OUT:"C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\DataCLI\DataCLI.exe" ^
REM    "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\DataCLI\DataCLI.lib" ^
REM    "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\Engine\Engine.lib" ^
REM    "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\Shared\Shared.lib"  ^
REM    "kernel32.lib" ^
REM    "libcmt.lib" ^
REM    /ENTRY:_DllMainCRTStartup /DEBUG ^
REM    /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\lib\x64" ^
REM    /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0\um\x64" ^
REM    /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0\ucrt\x64" ^
REM    /MACHINE:X64 ^
REM    /WX

REM "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\bin\Hostx64\x64\dumpbin.exe" ^
REM    /ALL ^
REM    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\lib\x64\msvcprt.lib" > dump_msvcprt.log
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\bin\Hostx64\x64\dumpbin.exe" ^
   /ALL ^
   "C:\Users\Thomas\Code\CubeWorldC++\Build\x64\dependencies\libyaml\0.2.2\Debug\OFF.lib" > dump_off.log

"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\bin\Hostx64\x64\link.exe" ^
   /WX /NOLOGO /INCREMENTAL:NO ^
   /OUT:"C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\DataCLI\DataCLI.exe" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\DataCLI\ConvertDocumentCommand.obj" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\DataCLI\ConvertModelCommand.obj" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\DataCLI\Database.obj" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\DataCLI\DumpCommand.obj" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\DataCLI\Main.obj" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\DataCLI\Scrambler.obj" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\Engine\Engine.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\Shared\Shared.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\RGBBinding\RGBBinding.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\RGBFileSystem\RGBFileSystem.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\RGBLogger\RGBLogger.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\RGBNetworking\RGBNetworking.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\RGBText\RGBText.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\freetype\freetype.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\glad\glad.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\glfw\glfw.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\libyaml\libyaml.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\lodepng\lodepng.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\rhea\rhea.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\sqlite\sqlite.lib" ^
   "C:\Users\Thomas\Code\CubeWorldC++\tmp\x64-Debug\zlib\zlib.lib" ^
   /DEBUG ^
   /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\lib\x64" ^
   /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0\um\x64" ^
   /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0\ucrt\x64" ^
   /MACHINE:X64 ^
   ComDlg32.Lib