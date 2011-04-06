
AG_PROJECT(TUTOOLS TUT)
AG_FILE(TUTOOLS_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TUTOOLS_FILES})

add_library (TUTools ${TUTOOLS_FILES})
target_link_libraries (TUTools Core)

