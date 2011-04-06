
AG_PROJECT(TUUNIVERSE TUU)
AG_FILE(TUUNIVERSE_FILES "")

#-----------------------------------------------------------------------------
#le define comptenant le nom du module utilise par le log
REMOVE_DEFINITIONS(-DLM_LOG_MODULE_NAME)
ADD_DEFINITIONS(-DLM_LOG_MODULE_NAME=L"TUUniverse")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TUUNIVERSE_FILES})

add_executable (TUUniverse ${TUUNIVERSE_FILES})
use_precompiled_header (TUUniverse)

target_link_libraries (TUUniverse Universe RendererStub AudioStub RendererSM2 GfxDx9 Window TUTools)
 
IF(CMAKE_COMPILER_IS_MINGW)
    target_link_libraries (TUUniverse libwinmm.a)
ENDIF(CMAKE_COMPILER_IS_MINGW)
