echo off

pushd ..
SET LOADED_SOURCE_DIR=%CD%
popd

pushd ..\Build
SET LOADED_BUILD_DIR=%CD%
popd

SET COMPILER_DIR_SHARED="VS2008_Shared"
SET GENERATOR="Visual Studio 9 2008"
SET BUILD_SHARED_LIBS="ON"

if not exist %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\ (mkdir %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\)
if not exist %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\Makefiles (mkdir %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\Makefiles)
if not exist %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe (mkdir %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe)
if not exist %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe\Debug (mkdir %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe\Debug)
if not exist %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe\Release (mkdir %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe\Release)
if not exist %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe\ReleaseDev (mkdir %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe\ReleaseDev)

pushd %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\Makefiles
cmake -G%GENERATOR% %LOADED_SOURCE_DIR% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%
popd

echo "DLL For Debug -----------------------------------------------------------"
xcopy /Y ..\LibsExternes\Dll\VC90\Win32\*.dll %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe\Debug

echo "DLL For Release ---------------------------------------------------------"
xcopy /Y ..\LibsExternes\Dll\VC90\Win32\*.dll %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe\Release

echo "DLL For ReleaseDev ------------------------------------------------------"
xcopy /Y ..\LibsExternes\Dll\VC90\Win32\*.dll %LOADED_BUILD_DIR%\%COMPILER_DIR_SHARED%\OutputExe\ReleaseDev

pause
