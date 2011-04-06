# Ce projet permet surtout de factoriser les options de compilation et de 
# pouvoir compiler tous les modules d'une seule traite.

project (ALL_MODULES)

# Liste des modules
add_subdirectory (Core)
add_subdirectory (Network)
add_subdirectory (Web)

if(WIN32 OR WIN64)
    add_subdirectory (Assets)
    add_subdirectory (Audio)
    add_subdirectory (Audio/AudioOpenAL)
    add_subdirectory (Audio/Stub)
    add_subdirectory (Gfx)
    add_subdirectory (Gfx/GfxDx9)
    add_subdirectory (GUI)
    add_subdirectory (Images)
    add_subdirectory (Packing)
    add_subdirectory (Renderer)
    add_subdirectory (Renderer/SM2)
    add_subdirectory (Renderer/Stub)
    add_subdirectory (Universe)
    add_subdirectory (Window)
    add_subdirectory (Workflow)
    add_subdirectory (Social)
endif(WIN32 OR WIN64)