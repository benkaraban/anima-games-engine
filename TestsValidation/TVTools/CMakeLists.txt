
AG_PROJECT(TVTOOLS TVT)
AG_FILE(TVTOOLS_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TVTOOLS_FILES})

add_library (TVTools ${TVTOOLS_FILES})
target_link_libraries (TVTools Window Renderer)
