
AG_PROJECT(SOCIAL SCL)

set (SOCIAL_ALL_PATH "/" )


#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(SOCIAL_ALL_PATH SOCIAL_INC_FILES SOCIAL_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Social SOCIAL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Social ${SOCIAL_INC_FILES} ${SOCIAL_SRC_FILES})
target_link_libraries (Social Core Web)

AG_PROJECT_END(Social Social)