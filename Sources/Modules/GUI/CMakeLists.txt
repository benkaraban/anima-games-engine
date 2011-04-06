AG_PROJECT(GUI GUI)

set (GUI_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(GUI_ALL_PATH GUI_INC_FILES GUI_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(GUI GUI_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (GUI ${GUI_INC_FILES} ${GUI_SRC_FILES})

target_link_libraries (GUI Core Assets Renderer Window)

AG_PROJECT_END(GUI GUI)