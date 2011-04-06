AG_PROJECT(PACKING PAK)

set (PACKING_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(PACKING_ALL_PATH PACKING_INC_FILES PACKING_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Packing PACKING_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Packing ${PACKING_INC_FILES} ${PACKING_SRC_FILES})

target_link_libraries (Packing Workflow Core Assets Images Universe GUI)

AG_PROJECT_END(Packing Packing)