
AG_PROJECT(TUWEB TUW)
AG_FILE(TUWEB_FILES "")

#-----------------------------------------------------------------------------
#le define comptenant le nom du module utilise par le log
REMOVE_DEFINITIONS(-DLM_LOG_MODULE_NAME)
ADD_DEFINITIONS(-DLM_LOG_MODULE_NAME=L"TUWeb")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TUWEB_FILES})

add_executable (TUWeb ${TUWEB_FILES})
use_precompiled_header (TUWeb)

target_link_libraries (TUWeb Web TUTools)
 
IF(CMAKE_COMPILER_IS_MINGW)
    target_link_libraries (TUWeb libwinmm.a)
ENDIF(CMAKE_COMPILER_IS_MINGW)
