
# Ce projet permet surtout de factoriser les options de compilation et de 
# pouvoir compiler tout d'une seule traite.

project (HOO)

# Liste des modules

add_subdirectory (HOOServer)
if(WIN32 OR WIN64)
    add_subdirectory (HOOClient)
    add_subdirectory (HOOUpdater)
endif(WIN32 OR WIN64)
add_subdirectory (HOOClientBot)
add_subdirectory (HOOClientStub)
add_subdirectory (HOOAdminProtocol)
add_subdirectory (HOOAdminTool)
add_subdirectory (HOOUserProtocol)
add_subdirectory (GameEngine)
add_subdirectory (HOOUpdaterProtocol)