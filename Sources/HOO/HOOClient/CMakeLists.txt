AG_PROJECT(HOOCLIENT HOC)

set (HOOCLIENT_ALL_PATH "/" )
list (APPEND HOOCLIENT_ALL_PATH "/GUI/")
list (APPEND HOOCLIENT_ALL_PATH "/InGame/")
list (APPEND HOOCLIENT_ALL_PATH "/Network/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(HOOCLIENT_ALL_PATH HOOCLIENT_INC_FILES HOOCLIENT_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(HOOClient HOOCLIENT_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

IF( WIN32 )
   SET( WIN32_EXECUTABLE WIN32 )
ENDIF( WIN32 )

add_executable (HeirsOfOlympus ${WIN32_EXECUTABLE} ${HOOCLIENT_INC_FILES} ${HOOCLIENT_SRC_FILES})
target_link_libraries (HeirsOfOlympus Universe RendererSM2 GfxDx9 GUI Window AudioOpenAL Network HOOUserProtocol GameEngine)
