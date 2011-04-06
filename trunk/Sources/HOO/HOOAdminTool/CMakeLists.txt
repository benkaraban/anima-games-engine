find_package(Qt4)

IF (QT_WRAP_CPP AND QT_WRAP_UI)
    AG_PROJECT(HOOADMINTOOL HAT)

    # on regroupe les paths
    set (HOOADMINTOOL_ALL_PATH "/" )
    list (APPEND HOOADMINTOOL_ALL_PATH "/Dialog/")
    list (APPEND HOOADMINTOOL_ALL_PATH "/ServerAnswerHandler/")
    
    #on configure QT
    AG_CONFIG_QT4(HOOADMINTOOL_ALL_PATH)

    #on regroupe l'ensemble des fichiers includes et sources
    ag_create_incFile_srcFile(HOOADMINTOOL_ALL_PATH HOOADMINTOOL_INC_FILES HOOADMINTOOL_SRC_FILES)
    
    #on regroupe les fichers sources avec les moc_cpp
    set(HOOADMINTOOL_ALL_SRC ${MOC_FILES_CPP})
    list (APPEND HOOADMINTOOL_ALL_SRC ${HOOADMINTOOL_SRC_FILES})

    #on configure le precompiled header
    AG_MAKE_PRECOMPILED_HEADERS(HOOAdminTool HOOADMINTOOL_ALL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

	IF( WIN32 )
		SET( WIN32_EXECUTABLE WIN32 )
	ENDIF( WIN32 )
	
    add_executable (HOOAdminTool ${WIN32_EXECUTABLE} ${HOOADMINTOOL_INC_FILES} ${HOOADMINTOOL_ALL_SRC})
    target_link_libraries (HOOAdminTool ${QT_LIBRARIES} Core QtToolbox Network HOOAdminProtocol)
    
ELSE  (QT_WRAP_CPP AND QT_WRAP_UI)
    MESSAGE(STATUS "HOOAdminTool : QT Not found... project skipped.")
ENDIF (QT_WRAP_CPP AND QT_WRAP_UI)