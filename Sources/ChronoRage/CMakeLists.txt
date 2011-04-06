
AG_PROJECT(CHRONORAGE CHR)

set (CHRONORAGE_ALL_PATH "/" )
list (APPEND CHRONORAGE_ALL_PATH "/Controls/")
list (APPEND CHRONORAGE_ALL_PATH "/Foes/")
list (APPEND CHRONORAGE_ALL_PATH "/Foes/Shots/")
list (APPEND CHRONORAGE_ALL_PATH "/Waves/")
list (APPEND CHRONORAGE_ALL_PATH "/Modes/")
list (APPEND CHRONORAGE_ALL_PATH "/MenuScreens/")
list (APPEND CHRONORAGE_ALL_PATH "/MenuScreens/GUI/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(CHRONORAGE_ALL_PATH CHRONORAGE_INC_FILES CHRONORAGE_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(ChronoRage CHRONORAGE_SRC_FILES)

file(GLOB CHRONORAGE_RC_FILES ChronoRage.rc)

IF( WIN32 )
   SET( WIN32_EXECUTABLE WIN32 )
ENDIF( WIN32 )


add_executable (ChronoRage ${WIN32_EXECUTABLE} ${CHRONORAGE_INC_FILES} ${CHRONORAGE_SRC_FILES} ${CHRONORAGE_RC_FILES})
target_link_libraries (ChronoRage Core Universe RendererSM2 GfxDx9 Window GUI AudioOpenAL winmm)

add_subdirectory (_Demo)

