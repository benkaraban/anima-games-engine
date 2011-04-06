AG_PROJECT(DRAFT DFT)

set (DRAFT_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(DRAFT_ALL_PATH DRAFT_INC_FILES DRAFT_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Draft DRAFT_SRC_FILES)

add_executable (Draft ${DRAFT_INC_FILES} ${DRAFT_SRC_FILES})
target_link_libraries (Draft Core)
