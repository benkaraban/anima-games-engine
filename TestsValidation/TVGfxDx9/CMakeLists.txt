
AG_PROJECT(TVGFXDX9 TVG)
AG_FILE(TVGfxDx9_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TVGfxDx9_FILES})

#-----------------------------------------------------------------------------
IF( WIN32 )
   SET( WIN32_EXECUTABLE WIN32 )
ENDIF( WIN32 )

add_executable (TVGfxDx9 ${WIN32_EXECUTABLE} ${TVGfxDx9_FILES})
use_precompiled_header (TVGfxDx9)

target_link_libraries (TVGfxDx9 GfxDx9 Core Window Images TUTools)

IF(CMAKE_COMPILER_IS_MINGW)
    target_link_libraries (libwinmm.a)
ELSE()
    target_link_libraries (winmm)
ENDIF(CMAKE_COMPILER_IS_MINGW)
