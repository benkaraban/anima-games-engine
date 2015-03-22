echo off
echo -----------------------------------------------------------------------
echo LOADED_SOURCE_DIR = %LOADED_SOURCE_DIR%
echo LOADED_BUILD_DIR  = %LOADED_BUILD_DIR%
echo -----------------------------------------------------------------------
if not defined LOADED_SOURCE_DIR (goto ERREUR_SOURCE_VAR)
if not defined LOADED_BUILD_DIR (goto ERREUR_BUILD_VAR)
if not exist %LOADED_SOURCE_DIR% (goto ERREUR_SRC_DIR)
if not exist %LOADED_BUILD_DIR% (goto ERREUR_BUILD_DIR)





:GCC_CONFIGURATION
SET CC=gcc
SET CXX=g++
SET CXX_FLAGS="-Wall -Wno-sign-compare -mthreads -msse -D__SSE__"
SET CXX_FLAGS_DEBUG="-gstabs -D_DEBUG"
SET CXX_FLAGS_RELEASE="-O3 -Winline --param large-function-growth=400"

SET DEBUG_STATIC_COMPILER_DIR="GCC_Debug_Static"
SET DEBUG_COMPILER_DIR="GCC_Debug"
SET RELEASE_COMPILER_DIR="GCC_Release"

SET GENERATOR="MinGW Makefiles"
goto LAUNCH_CMAKE





:LAUNCH_CMAKE
if not exist %LOADED_BUILD_DIR%\%DEBUG_STATIC_COMPILER_DIR%\ (mkdir %LOADED_BUILD_DIR%\%DEBUG_STATIC_COMPILER_DIR%\)
if not exist %LOADED_BUILD_DIR%\%DEBUG_STATIC_COMPILER_DIR%\Makefiles (mkdir %LOADED_BUILD_DIR%\%DEBUG_STATIC_COMPILER_DIR%\Makefiles)

if not exist %LOADED_BUILD_DIR%\%DEBUG_COMPILER_DIR%\ (mkdir %LOADED_BUILD_DIR%\%DEBUG_COMPILER_DIR%\)
if not exist %LOADED_BUILD_DIR%\%DEBUG_COMPILER_DIR%\Makefiles (mkdir %LOADED_BUILD_DIR%\%DEBUG_COMPILER_DIR%\Makefiles)

if not exist %LOADED_BUILD_DIR%\%RELEASE_COMPILER_DIR%\ (mkdir %LOADED_BUILD_DIR%\%RELEASE_COMPILER_DIR%\)
if not exist %LOADED_BUILD_DIR%\%RELEASE_COMPILER_DIR%\Makefiles (mkdir %LOADED_BUILD_DIR%\%RELEASE_COMPILER_DIR%\Makefiles)


pushd %LOADED_BUILD_DIR%\%DEBUG_STATIC_COMPILER_DIR%\Makefiles
SET BUILD_SHARED_LIBS="OFF"
cmake -G%GENERATOR% -DCMAKE_BUILD_TYPE=DEBUG %LOADED_SOURCE_DIR% -DCMAKE_CXX_FLAGS=%CXX_FLAGS% -DCMAKE_CXX_FLAGS_DEBUG=%CXX_FLAGS_DEBUG% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%
cmake -G%GENERATOR% -DCMAKE_BUILD_TYPE=DEBUG %LOADED_SOURCE_DIR% -DCMAKE_CXX_FLAGS=%CXX_FLAGS% -DCMAKE_CXX_FLAGS_DEBUG=%CXX_FLAGS_DEBUG% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%
popd


pushd %LOADED_BUILD_DIR%\%DEBUG_COMPILER_DIR%\Makefiles
SET BUILD_SHARED_LIBS="ON"
cmake -G%GENERATOR% -DCMAKE_BUILD_TYPE=DEBUG %LOADED_SOURCE_DIR% -DCMAKE_CXX_FLAGS=%CXX_FLAGS% -DCMAKE_CXX_FLAGS_DEBUG=%CXX_FLAGS_DEBUG% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%
cmake -G%GENERATOR% -DCMAKE_BUILD_TYPE=DEBUG %LOADED_SOURCE_DIR% -DCMAKE_CXX_FLAGS=%CXX_FLAGS% -DCMAKE_CXX_FLAGS_DEBUG=%CXX_FLAGS_DEBUG% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%
popd


pushd %LOADED_BUILD_DIR%\%RELEASE_COMPILER_DIR%\Makefiles
SET BUILD_SHARED_LIBS="ON"
cmake -G%GENERATOR% -DCMAKE_BUILD_TYPE=RELEASE %LOADED_SOURCE_DIR% -DCMAKE_CXX_FLAGS=%CXX_FLAGS% -DCMAKE_CXX_FLAGS_RELEASE=%CXX_FLAGS_RELEASE% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%
cmake -G%GENERATOR% -DCMAKE_BUILD_TYPE=RELEASE %LOADED_SOURCE_DIR% -DCMAKE_CXX_FLAGS=%CXX_FLAGS% -DCMAKE_CXX_FLAGS_RELEASE=%CXX_FLAGS_RELEASE% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%
popd

goto end


:ERREUR_SOURCE_VAR
echo ERROR : the LOADED_SOURCE_DIR env variable is not set
goto end

:ERREUR_BUILD_VAR
echo ERROR : the LOADED_BUILD_DIR env variable is not set
goto end

:ERREUR_SRC_DIR
echo ERROR : "%LOADED_SOURCE_DIR%" directory not found
goto end

:ERREUR_BUILD_DIR
echo ERROR : "%LOADED_BUILD_DIR%" directory not found
goto end

:end
pause