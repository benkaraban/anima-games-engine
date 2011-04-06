
AG_PROJECT(TUSOCIAL TUS)
AG_FILE(TUSOCIAL_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TUSOCIAL_FILES})

add_executable (TUSocial ${TUSOCIAL_FILES})
use_precompiled_header (TUSocial)
target_link_libraries (TUSocial Social Core TUTools)