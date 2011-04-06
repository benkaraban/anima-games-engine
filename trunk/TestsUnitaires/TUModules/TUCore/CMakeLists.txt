
AG_PROJECT(TUCORE TUC)
AG_FILE(TUCORE_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TUCORE_FILES})

add_executable (TUCore ${TUCORE_FILES})
use_precompiled_header (TUCore)

if(WIN32 OR WIN64)
    target_link_libraries (TUCore Core TUTools winmm)
else()
    target_link_libraries (TUCore Core TUTools)
endif(WIN32 OR WIN64)
