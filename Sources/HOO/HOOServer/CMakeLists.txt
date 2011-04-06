AG_PROJECT(HOOSERVER HOS)

set (HOOSERVER_ALL_PATH "/" )
list (APPEND HOOSERVER_ALL_PATH "/Database/")
list (APPEND HOOSERVER_ALL_PATH "/NetworkMessageHandler/")
list (APPEND HOOSERVER_ALL_PATH "/AdminMessageHandler/")
list (APPEND HOOSERVER_ALL_PATH "/UserMessageHandler/")
list (APPEND HOOSERVER_ALL_PATH "/UpdaterMessageHandler/")
list (APPEND HOOSERVER_ALL_PATH "/UserMessageHandler/LaunchMatch/")
list (APPEND HOOSERVER_ALL_PATH "/UserMessageHandler/GameMessage/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(HOOSERVER_ALL_PATH HOOSERVER_INC_FILES HOOSERVER_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(HOOServer HOOSERVER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_executable (HOOServer ${HOOSERVER_INC_FILES} ${HOOSERVER_SRC_FILES})
target_link_libraries (HOOServer Core Network HOOAdminProtocol HOOUserProtocol HOOUpdaterProtocol GameEngine Web)
