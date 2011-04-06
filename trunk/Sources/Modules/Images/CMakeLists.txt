AG_PROJECT(IMAGES IMG)

set (IMAGES_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(IMAGES_ALL_PATH IMAGES_INC_FILES IMAGES_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Images IMAGES_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Images ${IMAGES_INC_FILES} ${IMAGES_SRC_FILES})
target_link_libraries (Images Core FreeImage cv200 cxcore200)

AG_PROJECT_END(Images Images)