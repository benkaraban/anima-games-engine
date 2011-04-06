#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#définie les types de fichiers associes a un projet
macro(AG_FILE AG_FILE_NAME AG_FILE_PATH)
    file(GLOB ${AG_FILE_NAME} ${AG_FILE_PATH}*.h ${AG_FILE_PATH}*.inl ${AG_FILE_PATH}*.txx ${AG_FILE_PATH}*.cpp)
endmacro()
#----------------------------------------------------------------------------
#définie les types de fichiers associes a un projet en separant les fichier include des fichier source
macro(AG_FILE_INC_SRC AG_FILE_INC_NAME AG_FILE_SRC_NAME AG_FILE_PATH)
    file(GLOB ${AG_FILE_INC_NAME} ${AG_FILE_PATH}*.h ${AG_FILE_PATH}*.inl ${AG_FILE_PATH}*.txx)
    file(GLOB ${AG_FILE_SRC_NAME} ${AG_FILE_PATH}*.cpp)
endmacro()
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
#        Gestion API dllexport/dllimport
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
#permet de definir une LM_API_truc en export
macro(ADD_DEF_LM_API_EXPORT AG_LM_API)
    if(BUILD_SHARED_LIBS)
        if(WIN32 OR WIN64)
            ADD_DEFINITIONS(-DLM_API_${AG_LM_API}=__declspec\(dllexport\))
        else()
            ADD_DEFINITIONS(-DLM_API_${AG_LM_API}= )
        endif()
    else()
        ADD_DEFINITIONS(-DLM_API_${AG_LM_API}= )
    endif()
endmacro()
#-----------------------------------------------------------------------------
#permet de definir une LM_API_truc en import
macro(ADD_DEF_LM_API_IMPORT AG_LM_API)
    if(BUILD_SHARED_LIBS)
        if(WIN32 OR WIN64)
            ADD_DEFINITIONS(-DLM_API_${AG_LM_API}=__declspec\(dllimport\))
        else()
            ADD_DEFINITIONS(-DLM_API_${AG_LM_API}= )
        endif()
    else()
        ADD_DEFINITIONS(-DLM_API_${AG_LM_API}= )
    endif()
endmacro()
#-----------------------------------------------------------------------------
#permet de definir l'ensemble des define specifique a un projet
macro(AG_MODULE_IMPORT_EXPORT AG_PROJECT_SHORT_NAME AG_MODULE_SHORT_NAME)
    if("${AG_PROJECT_SHORT_NAME}" STREQUAL "${AG_MODULE_SHORT_NAME}")
        ADD_DEF_LM_API_EXPORT(${AG_MODULE_SHORT_NAME})
    else()
        ADD_DEF_LM_API_IMPORT(${AG_MODULE_SHORT_NAME})
    endif()
endmacro()
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
#        add_library
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
function (ag_create_incFile_srcFile IN_LIST_PATH OUT_ALL_INC_FILES OUT_ALL_SRC_FILES)
    set(ALL_INC_FILES "")
    set(ALL_SRC_FILES "")
    
    list (SORT ${IN_LIST_PATH})
    
    foreach(iPath IN LISTS ${IN_LIST_PATH})
        #on supprime le premier caracter '/'
        STRING(REGEX MATCH "[^^/].*" value1 ${iPath})    
        
        #on recupere les fichier INC et SRC du path
        if ("${value1}" STREQUAL "")
            AG_FILE_INC_SRC(INC_FILE_NAME SRC_FILE_NAME "")
        else ()
            AG_FILE_INC_SRC(INC_FILE_NAME SRC_FILE_NAME ${value1})
        endif()

        #creation des source_groupe
        STRING(REGEX REPLACE "[./]" "\\\\\\\\" SOURCE_GROUP_PATH ${iPath})    #on remplace  '/' par '\'
        SOURCE_GROUP(${SOURCE_GROUP_PATH} FILES ${INC_FILE_NAME} ${SRC_FILE_NAME})
        
        #on ajoute les fichiers inc et src 
        list(APPEND ALL_INC_FILES ${INC_FILE_NAME})
        list(APPEND ALL_SRC_FILES ${SRC_FILE_NAME})
    endforeach()
    
    set(${OUT_ALL_INC_FILES} ${ALL_INC_FILES} PARENT_SCOPE)
    set(${OUT_ALL_SRC_FILES} ${ALL_SRC_FILES} PARENT_SCOPE)
endfunction()
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
#        precompiled header
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
#AG_MAKE_PRECOMPILED_HEADERS
#configure le projet pour utilise un fichier de precompilation
#param1: IN_AG_PROJECT_NAME : le nom du projet (ex:Core)
#param2: IN_OUT_SRC_FILES : la liste de tous les fichier cpp du projet
#param3:optionnel : IN_PCH_NAME_ABS_PATH : le nom du fichier header utilise pour generer l'header precompile
#si le param3 n'est pas specifie on utilise le header par defaut ${PRELUDE_H_ABS_PATH}
macro(AG_MAKE_PRECOMPILED_HEADERS IN_AG_PROJECT_NAME IN_OUT_SRC_FILES)
    #on verifie si un fichier particulier a ete donne pour genere le header precompile
    set(IN_PCH_NAME_ABS_PATH ${ARGV2})
    if ("${IN_PCH_NAME_ABS_PATH}" STREQUAL "")
        set(IN_PCH_NAME_ABS_PATH ${PRELUDE_H_ABS_PATH})
    endif()
    
    if(${ENABLE_PRECOMPILED_HEADER} STREQUAL "OFF")
    else()
        #on configure chaque fichier cpp pour utiliser le header precompiler et include le fichier h
        foreach(source_file ${${IN_OUT_SRC_FILES}})
            set_source_files_properties(
            ${source_file}
            PROPERTIES COMPILE_FLAGS
            "/Yu\"${IN_PCH_NAME_ABS_PATH}\" /FI\"${IN_PCH_NAME_ABS_PATH}\""
            )
        endforeach()

        #on construit le nom du fichier cpp du header precompile
        get_filename_component(PCH_BASE_NAME ${IN_PCH_NAME_ABS_PATH} NAME_WE)    #le nom du fichier sans l'extention
        set(PROJECT_PRELUDE_CPP ${CMAKE_CURRENT_BINARY_DIR}/${IN_AG_PROJECT_NAME}${PCH_BASE_NAME}.cpp)
        
        #on cree le fichier cpp qui va permettre de creer l'header precompile
        #on check si le fichier exist deja
            #-1 on verifie si le fichier exist
        get_filename_component(PROJECT_PRELUDE_CPP_PATH ${PROJECT_PRELUDE_CPP} PATH)
        get_filename_component(PROJECT_PRELUDE_CPP_NAME ${PROJECT_PRELUDE_CPP} NAME)
        unset (PROJECT_PRELUDE_CPP_ISCREATED CACHE)    #pour find_file on a besoin de unset la <VAR>
        find_file(PROJECT_PRELUDE_CPP_ISCREATED NAMES ${PROJECT_PRELUDE_CPP_NAME} PATHS ${PROJECT_PRELUDE_CPP_PATH} NO_DEFAULT_PATH)
        
            #-2 on decide si on a besoin de refaire le fichier
        set (CREATE_PROJECT_PRELUDE_CPP "OFF")
        if ("${PROJECT_PRELUDE_CPP_ISCREATED}" STREQUAL "PROJECT_PRELUDE_CPP_ISCREATED-NOTFOUND")
            set (CREATE_PROJECT_PRELUDE_CPP "ON")   #si le fichier n'existe pas on va le creer
        else()
            file(READ ${PROJECT_PRELUDE_CPP} PRELUDE_CPP_CONTENT)
            
            if("${PRELUDE_CPP_CONTENT}" STREQUAL "#include <${IN_PCH_NAME_ABS_PATH}>\n")
                set (CREATE_PROJECT_PRELUDE_CPP "OFF")  #le fichier est identique on ne le reecrit pas
            else()
                set (CREATE_PROJECT_PRELUDE_CPP "ON")   #le fichier est different on le recre
            endif()
        endif()
        
        #si besoin on creele fichier
        if ("${CREATE_PROJECT_PRELUDE_CPP}" STREQUAL "ON")
            file(WRITE ${PROJECT_PRELUDE_CPP} "#include <${IN_PCH_NAME_ABS_PATH}>\n")
            set_source_files_properties(${PROJECT_PRELUDE_CPP}  PROPERTIES COMPILE_FLAGS "/Yc\"${IN_PCH_NAME_ABS_PATH}\"")
        endif()

        SOURCE_GROUP("_Generated" FILES ${PROJECT_PRELUDE_CPP})
        
        #on ajoute le fichier PROJECT_PRELUDE_CPP au fichier src du projet
        set(${IN_OUT_SRC_FILES} ${${IN_OUT_SRC_FILES}} ${PROJECT_PRELUDE_CPP} )
        
        #on configure le projet visual
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yc\"${IN_PCH_NAME_ABS_PATH}\"")
    endif()
endmacro()
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
#        utilisation de Qt4
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------
macro(AG_CONFIG_QT4 IN_LIST_PATH)
    set (NEW_PATH "")
    foreach(iPath IN LISTS ${IN_LIST_PATH})
        #on supprime le premier caracter '/'
        STRING(REGEX MATCH "[^^/].*" value1 ${iPath})
        list(APPEND NEW_PATH ${value1}*.moc.h)
    endforeach()
    
    # Qt Includes
    include(${QT_USE_FILE})
    
    #On ajoute le répertoire de génération pour les fichiers générés
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    
    # Generating MOC's
    file(GLOB MOC_FILES ${NEW_PATH})
    qt4_wrap_cpp(MOC_FILES_CPP ${MOC_FILES})
    
    # Qt Definitions
    add_definitions(${QT_DEFINITIONS})
    
    # Source Groupe
    SOURCE_GROUP("_Generated" FILES ${MOC_FILES_CPP})
endmacro()
#----------------------------------------------------------------------------
#----------------------------------------------------------------------------