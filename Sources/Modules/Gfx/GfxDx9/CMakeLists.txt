AG_PROJECT(GFXDX9 GX9)
set (GFXDX9_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(GFXDX9_ALL_PATH GFXDX9_INC_FILES GFXDX9_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(GfxDx9 GFXDX9_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (GfxDx9 ${GFXDX9_INC_FILES} ${GFXDX9_SRC_FILES})
target_link_libraries (GfxDx9 Gfx Core DxErr d3d9 dxguid Wbemuuid.lib)
target_link_libraries (GfxDx9 debug d3dx9d)
target_link_libraries (GfxDx9 optimized d3dx9)

AG_PROJECT_END(GfxDx9 Gfx)