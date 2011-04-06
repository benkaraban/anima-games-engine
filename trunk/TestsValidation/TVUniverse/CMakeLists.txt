
AG_PROJECT(TVUNIVERSE TVU)
AG_FILE(TVUNIVERSE_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TVUNIVERSE_FILES})

#-----------------------------------------------------------------------------
IF( WIN32 )
   SET( WIN32_EXECUTABLE WIN32 )
ENDIF( WIN32 )

add_executable (TVUniverse ${WIN32_EXECUTABLE} ${TVUNIVERSE_FILES})

use_precompiled_header (TVUniverse)
target_link_libraries (TVUniverse Universe RendererSM2 GfxDx9 Workflow TUTools Window Packing TVTools AudioOpenAL)


IF(CMAKE_COMPILER_IS_MINGW)
    target_link_libraries (TVUniverse libwinmm.a)
ELSE()
    target_link_libraries (TVUniverse winmm)
ENDIF(CMAKE_COMPILER_IS_MINGW)


