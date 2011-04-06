
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/TVTools)

add_subdirectory(TVNetwork)

IF(WIN32 OR WIN64)
    add_subdirectory(TVTools)
	add_subdirectory(TVAudioOpenAL)
    add_subdirectory(TVGfxDx9)
    add_subdirectory(TVGUI)
    add_subdirectory(TVRenderer)
    add_subdirectory(TVUniverse)
ENDIF(WIN32 OR WIN64)