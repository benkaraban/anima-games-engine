AG_PROJECT(HOOCLIENTSTUB HOC)

set (HOOCLIENTSTUB_ALL_PATH "/" )
list (APPEND HOOCLIENTSTUB_ALL_PATH "/ServerAnswerHandler/")
list (APPEND HOOCLIENTSTUB_ALL_PATH "/ServerAnswerHandler/LaunchMatchHandler/")
list (APPEND HOOCLIENTSTUB_ALL_PATH "/ServerAnswerHandler/GameMessageHandler/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(HOOCLIENTSTUB_ALL_PATH HOOCLIENTSTUB_INC_FILES HOOCLIENTSTUB_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(HOOClientStub HOOCLIENTSTUB_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_executable (HOOClientStub ${HOOCLIENTSTUB_INC_FILES} ${HOOCLIENTSTUB_SRC_FILES})
target_link_libraries (HOOClientStub Core Network HOOUserProtocol)
