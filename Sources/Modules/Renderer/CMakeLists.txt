AG_PROJECT(RENDERER RDR)

set (RENDERER_ALL_PATH "/" )
list (APPEND RENDERER_ALL_PATH "/Common/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(RENDERER_ALL_PATH RENDERER_INC_FILES RENDERER_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Renderer RENDERER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Renderer ${RENDERER_INC_FILES} ${RENDERER_SRC_FILES})
target_link_libraries (Renderer Core Assets Gfx)

AG_PROJECT_END(Renderer Renderer)