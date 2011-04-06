AG_PROJECT(UNIVERSE UNI)

set (UNIVERSE_ALL_PATH "/" )
list (APPEND UNIVERSE_ALL_PATH "/Lua/")
list (APPEND UNIVERSE_ALL_PATH "/Tasks/")
list (APPEND UNIVERSE_ALL_PATH "/Particles/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(UNIVERSE_ALL_PATH UNIVERSE_INC_FILES UNIVERSE_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Universe UNIVERSE_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Universe ${UNIVERSE_INC_FILES} ${UNIVERSE_SRC_FILES})
target_link_libraries (Universe Core Assets Renderer Audio lua52)

AG_PROJECT_END(Universe Universe)