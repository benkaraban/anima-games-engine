
AG_PROJECT(TUASSETS TUA)
AG_FILE(TUASSETS_FILES "")

#-----------------------------------------------------------------------------
#on regroupe les fichiers
SOURCE_GROUP("" FILES ${TUASSETS_FILES})

add_executable (TUAssets ${TUASSETS_FILES})
use_precompiled_header (TUAssets)
target_link_libraries (TUAssets Assets Core Images Workflow TUTools)

