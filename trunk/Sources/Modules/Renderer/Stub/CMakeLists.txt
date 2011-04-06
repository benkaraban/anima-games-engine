
AG_PROJECT(RENDERERSTUB RDS)

set (RENDERERSTUB_ALL_PATH "/" )


#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(RENDERERSTUB_ALL_PATH RENDERERSTUB_INC_FILES RENDERERSTUB_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(RendererStub RENDERERSTUB_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (RendererStub ${RENDERERSTUB_INC_FILES} ${RENDERERSTUB_SRC_FILES})
target_link_libraries (RendererStub Core Assets Renderer)

