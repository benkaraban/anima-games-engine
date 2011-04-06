AG_PROJECT(HOOUPDATER HUP)

set (HOOUPDATER_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(HOOUPDATER_ALL_PATH HOOUPDATER_INC_FILES HOOUPDATER_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(HOOUpdater HOOUPDATER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

file(GLOB RC_FILES HooUpdater.rc)

IF( WIN32 )
   SET( WIN32_EXECUTABLE WIN32 )
ENDIF( WIN32 )

add_executable (HOOUpdater ${WIN32_EXECUTABLE} ${HOOUPDATER_INC_FILES} ${HOOUPDATER_SRC_FILES} ${RC_FILES})
target_link_libraries (HOOUpdater HOOUpdaterProtocol Core Network Web)
