
AG_PROJECT(TVGUI TVI)
AG_FILE(TVGUI_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TVGUI_FILES})

#-----------------------------------------------------------------------------
IF( WIN32 )
   SET( WIN32_EXECUTABLE WIN32 )
ENDIF( WIN32 )

add_executable (TVGUI ${WIN32_EXECUTABLE} ${TVGUI_FILES})

use_precompiled_header (TVGUI)
target_link_libraries (TVGUI Universe RendererSM2 GfxDx9 Workflow TUTools Window Packing GUI TVTools)


IF(CMAKE_COMPILER_IS_MINGW)
    target_link_libraries (TVGUI libwinmm.a)
ELSE()
    target_link_libraries (TVGUI winmm)
ENDIF(CMAKE_COMPILER_IS_MINGW)


