find_package(Qt4)

IF (QT_WRAP_CPP AND QT_WRAP_UI)
    AG_PROJECT(NYX NYX)
    
    #version
    set(NYX_VERSION "0.1.22")

    string(REGEX MATCHALL "[0-9]" NYX_VERSION_PARTS "${NYX_VERSION}")

    list(GET NYX_VERSION_PARTS 0 NYX_VERSION_MAJOR)
    list(GET NYX_VERSION_PARTS 1 NYX_VERSION_MINOR)
    list(GET NYX_VERSION_PARTS 2 NYX_VERSION_PATCH)

    ADD_DEFINITIONS(-DNYX_VERSION="${NYX_VERSION}")

    # on regroupe les paths
    set (NYX_ALL_PATH "/" )
    
    #on configure QT
    AG_CONFIG_QT4(NYX_ALL_PATH)
    # Generating QRC's
    file(GLOB QRC_FILES resources.qrc)
    qt4_add_resources(QRC_FILES_CPP ${QRC_FILES})
    
    
    SOURCE_GROUP("_Generated" FILES ${QRC_FILES_CPP})
    
    #on regroupe l'ensemble des fichiers includes et sources
    ag_create_incFile_srcFile(NYX_ALL_PATH NYX_INC_FILES NYX_SRC_FILES)
    
    #on regroupe les fichers sources avec les moc_cpp
    set(NYX_ALL_SRC ${MOC_FILES_CPP})
    list (APPEND NYX_ALL_SRC ${QRC_FILES_CPP})
    list (APPEND NYX_ALL_SRC ${NYX_SRC_FILES})

    #on configure le precompiled header
	AG_MAKE_PRECOMPILED_HEADERS(Nyx NYX_ALL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)
	
	file(GLOB RC_FILES resources.rc)
	
	IF( WIN32 )
		SET( WIN32_EXECUTABLE WIN32 )
	ENDIF( WIN32 )

    add_executable (Nyx ${WIN32_EXECUTABLE} ${NYX_INC_FILES} ${NYX_ALL_SRC} ${RC_FILES})
    target_link_libraries(Nyx ${QT_LIBRARIES} Core EPI)
    
    
ELSE  (QT_WRAP_CPP AND QT_WRAP_UI)
    MESSAGE(STATUS "Nyx : QT Not found... project skipped.")
ENDIF (QT_WRAP_CPP AND QT_WRAP_UI)

AG_PROJECT_END(Nyx Nyx)            