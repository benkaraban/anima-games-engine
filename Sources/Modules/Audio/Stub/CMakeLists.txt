
AG_PROJECT(AUDIOSTUB ADS)

set (AUDIOSTUB_ALL_PATH "/" )


#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(AUDIOSTUB_ALL_PATH AUDIOSTUB_INC_FILES AUDIOSTUB_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(AudioStub AUDIOSTUB_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (AudioStub ${AUDIOSTUB_INC_FILES} ${AUDIOSTUB_SRC_FILES})
target_link_libraries (AudioStub Core Audio)

