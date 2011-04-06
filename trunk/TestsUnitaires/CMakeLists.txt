
# Ce projet permet surtout de factoriser les options de compilation et de 
# pouvoir compiler tout d'une seule traite.

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/TUTools)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/TUModules)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/TUHOO)

# Liste des modules

add_subdirectory (TUModules)
add_subdirectory (TUTools)
add_subdirectory (TUHOO)

