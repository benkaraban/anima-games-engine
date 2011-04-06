AG_PROJECT(NETWORK NET)

set (NETWORK_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(NETWORK_ALL_PATH NETWORK_INC_FILES NETWORK_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Network NETWORK_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Network ${NETWORK_INC_FILES} ${NETWORK_SRC_FILES})

IF(WIN32 OR WIN64)
	target_link_libraries (Network Core Ws2_32.lib)
ELSE(WIN32 OR WIN64)
	target_link_libraries (Network Core)
ENDIF(WIN32 OR WIN64)
