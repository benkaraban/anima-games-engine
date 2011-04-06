AG_PROJECT(GFX GFX)

set (GFX_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(GFX_ALL_PATH GFX_INC_FILES GFX_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Gfx GFX_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Gfx ${GFX_INC_FILES} ${GFX_SRC_FILES})
target_link_libraries (Gfx Core)

AG_PROJECT_END(Gfx Gfx)