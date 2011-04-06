
AG_PROJECT(TUNETWORKENGINE TUN)
AG_FILE(TUNETWORKENGINE_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TUNETWORKENGINE_FILES})

add_executable (TUNetwork ${TUNETWORKENGINE_FILES})
target_link_libraries (TUNetwork Network Core TUTools)
use_precompiled_header (TUNetwork)

