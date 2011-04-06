AG_PROJECT(ASSETS ASS)

set (ASSETS_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(ASSETS_ALL_PATH ASSETS_INC_FILES ASSETS_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Assets ASSETS_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Assets ${ASSETS_INC_FILES} ${ASSETS_SRC_FILES})
target_link_libraries (Assets Core)

AG_PROJECT_END(Assets Assets)