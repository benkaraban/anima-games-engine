
AG_PROJECT(TVRENDERER TVR)
AG_FILE(TVRENDERER_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TVRENDERER_FILES})

#-----------------------------------------------------------------------------
IF( WIN32 )
   SET( WIN32_EXECUTABLE WIN32 )
ENDIF( WIN32 )

add_executable (TVRenderer ${WIN32_EXECUTABLE} ${TVRENDERER_FILES})

use_precompiled_header (TVRenderer)

target_link_libraries (TVRenderer RendererSM2 GfxDx9 Workflow Window TUTools TVTools)

IF(CMAKE_COMPILER_IS_MINGW)
    target_link_libraries (libwinmm.a)
ELSE()
    target_link_libraries (winmm)
ENDIF(CMAKE_COMPILER_IS_MINGW)
