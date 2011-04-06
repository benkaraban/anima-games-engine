AG_PROJECT(AUDIO AUD)

set (AUDIO_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(AUDIO_ALL_PATH AUDIO_INC_FILES AUDIO_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Audio AUDIO_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Audio ${AUDIO_INC_FILES} ${AUDIO_SRC_FILES})
target_link_libraries (Audio Core)

AG_PROJECT_END(Audio Audio)