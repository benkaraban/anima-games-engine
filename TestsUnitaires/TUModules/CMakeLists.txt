
add_subdirectory (TUCore)
add_subdirectory (TUNetwork)
if(WIN32 OR WIN64)
    add_subdirectory (TUAssets)
    add_subdirectory (TUUniverse)
    add_subdirectory (TUSocial)
    add_subdirectory (TUWeb)
endif(WIN32 OR WIN64)