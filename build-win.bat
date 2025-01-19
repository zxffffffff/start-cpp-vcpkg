@echo off
setlocal EnableDelayedExpansion

set root_path=%~dp0
cd %root_path%

:: 初始化 vcpkg
set vcpkg_path=%root_path%\vcpkg
set vcpkg_exe=%vcpkg_path%\vcpkg.exe
echo vcpkg_path: %vcpkg_path%
echo vcpkg_exe: %vcpkg_exe%
if not exist %vcpkg_exe% (
  set vcpkg_bat=%vcpkg_path%\bootstrap-vcpkg.bat
  echo vcpkg_bat: !vcpkg_bat!
  call !vcpkg_bat!
)

:: 可选vcpkg配置版本
:: x64-windows
:: x64-windows-static
:: x64-windows-static-md
:: x86-windows
:: x86-windows-static
:: x86-windows-static-md
set VCPKG_TARGET_TRIPLET=x64-windows
echo VCPKG_TARGET_TRIPLET: %VCPKG_TARGET_TRIPLET%

:: 选择MSVC运行时库
:: /MD  默认 MultiThreadedDLL
:: /MDd 默认 MultiThreadedDebugDLL
:: /MT       MultiThreaded
:: /MTd      MultiThreadedDebug
set CMAKE_MSVC_RUNTIME_LIBRARY="MultiThreadedDebugDLL"
echo CMAKE_MSVC_RUNTIME_LIBRARY: %CMAKE_MSVC_RUNTIME_LIBRARY%

:: 配置 Debug/Release
set config=Debug
set build_path=build-%VCPKG_TARGET_TRIPLET%-%config%
set install_path=install-%VCPKG_TARGET_TRIPLET%-%config%
echo config: %config%
echo build_path: %build_path%
echo install_path: %install_path%

if EXIST %install_path% (
    RMDIR /Q /S %install_path%
)

:: 可选编译器版本 -G
:: Visual Studio 17 2022
:: Visual Studio 16 2019
:: Visual Studio 15 2017
:: Visual Studio 14 2015
:: 可选编译32/64位 -A
:: Win32
:: x64
:: ARM
:: ARM64
echo cmake %build_path% ...
cmake -B %build_path% -S . -DCMAKE_INSTALL_PREFIX=%install_path% -DVCPKG_TARGET_TRIPLET=%VCPKG_TARGET_TRIPLET% -DCMAKE_MSVC_RUNTIME_LIBRARY=%CMAKE_MSVC_RUNTIME_LIBRARY%
IF %ERRORLEVEL% NEQ 0 (
    echo cmake %build_path% error:%ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

echo cmake %install_path% ...
cmake --build %build_path% --target install --config %config%
IF %ERRORLEVEL% NEQ 0 (
    echo cmake %install_path% error:%ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

:: 拷贝vcpkg第三方库
set vcpkg_installed=%build_path%\vcpkg_installed\%VCPKG_TARGET_TRIPLET%
echo vcpkg_installed: %vcpkg_installed%
:: 可选 xcopy /s /i /y %vcpkg_installed%\include %install_path%\include
if "%config%"=="Debug" (
  xcopy /s /i /y %vcpkg_installed%\debug\bin\*.dll %install_path%\bin
  xcopy /s /i /y %vcpkg_installed%\debug\lib\*.lib %install_path%\lib
) else (
  xcopy /s /i /y %vcpkg_installed%\bin\*.dll %install_path%\bin
  xcopy /s /i /y %vcpkg_installed%\lib\*.lib %install_path%\lib
)

:: run （也可使用 ctest）
echo run test
:: %install_path%\bin\sample-main.exe
:: %install_path%\bin\sample-db-test.exe
:: %install_path%\bin\sample-datacenter-test.exe
:: %install_path%\bin\sample-tools-test.exe
:: %install_path%\bin\sample-net-test.exe
ctest
