AG_PROJECT(RENDERERSM2 SM2)

set (RENDERERSM2_ALL_PATH "/" )
list (APPEND RENDERERSM2_ALL_PATH "/SM0/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(RENDERERSM2_ALL_PATH RENDERERSM2_INC_FILES RENDERERSM2_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(RendererSM2 RENDERERSM2_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (RendererSM2 ${RENDERERSM2_INC_FILES} ${RENDERERSM2_SRC_FILES})
target_link_libraries (RendererSM2 Core Assets Renderer)

AG_PROJECT_END(RendererSM2 Renderer)