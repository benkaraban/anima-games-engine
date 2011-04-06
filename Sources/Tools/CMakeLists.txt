# Ce projet permet surtout de factoriser les options de compilation et de 
# pouvoir compiler tous les tools d'une seule traite.

#if(QT_WRAP_CPP AND QT_WRAP_UI)

    project (ALL_TOOLS)

    if(WIN32 OR WIN64)
        add_subdirectory (QtToolbox)
        add_subdirectory (EPI)
        add_subdirectory (Nyx)
        add_subdirectory (ShaderCompiler)
    endif(WIN32 OR WIN64)
    
#endif(QT_WRAP_CPP AND QT_WRAP_UI)