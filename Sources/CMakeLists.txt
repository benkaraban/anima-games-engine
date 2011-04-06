
# Ce projet permet surtout de factoriser les options de compilation et de 
# pouvoir compiler tout d'une seule traite.

project (FULL_SOURCES )

# Liste des modules

add_subdirectory (Modules)
add_subdirectory (Tools)
add_subdirectory (Draft)
add_subdirectory (HOO)
if(WIN32 OR WIN64)
    add_subdirectory (ChronoRage)
endif(WIN32 OR WIN64)

