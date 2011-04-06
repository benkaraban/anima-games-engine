AG_PROJECT(GameEngine HGE)

set (HGE_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(HGE_ALL_PATH HGE_INC_FILES HGE_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(GameEngine HGE_SRC_FILES)

add_library (GameEngine ${HGE_INC_FILES} ${HGE_SRC_FILES})
target_link_libraries (GameEngine Core)
