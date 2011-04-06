AG_PROJECT(WINDOW WIN)

set (WINDOW_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(WINDOW_ALL_PATH WINDOW_INC_FILES WINDOW_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Window WINDOW_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Window ${WINDOW_INC_FILES} ${WINDOW_SRC_FILES})
target_link_libraries (Window Core dinput8 dxguid xinput DxErr)

AG_PROJECT_END(Window Window)