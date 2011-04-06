AG_PROJECT(AUDIOOPENAL OAL)

set (AUDIOOPENAL_ALL_PATH "/" )

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(AUDIOOPENAL_ALL_PATH AUDIOOPENAL_INC_FILES AUDIOOPENAL_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(AudioOpenAL AUDIOOPENAL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (AudioOpenAL ${AUDIOOPENAL_INC_FILES} ${AUDIOOPENAL_SRC_FILES})
target_link_libraries (AudioOpenAL Audio Core OpenAL32)

AG_PROJECT_END(AudioOpenAL Audio)