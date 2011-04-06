
AG_PROJECT(TUGAMEENGINE TUG)
AG_FILE(TUGAMEENGINE_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TUGAMEENGINE_FILES})

add_executable (TUGameEngine ${TUGAMEENGINE_FILES})
target_link_libraries (TUGameEngine GameEngine Core TUTools)
use_precompiled_header (TUGameEngine)

