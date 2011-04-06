find_package(Qt4)

IF (QT_WRAP_CPP AND QT_WRAP_UI)
    AG_PROJECT(EPI EPI)
    
    # on regroupe les paths
    set (EPI_ALL_PATH "/" )
    list (APPEND EPI_ALL_PATH "/Document/")
    list (APPEND EPI_ALL_PATH "/Document/ContentInfo/")
    list (APPEND EPI_ALL_PATH "/Document/Properties/")
    list (APPEND EPI_ALL_PATH "/Document/Properties/PtyNodeEmitter/")
    list (APPEND EPI_ALL_PATH "/Document/Properties/PtyTrajectory/")
    list (APPEND EPI_ALL_PATH "/GUI/")
    list (APPEND EPI_ALL_PATH "/GUI/Widget/")
    list (APPEND EPI_ALL_PATH "/GUI/VFSExplorer/")
    list (APPEND EPI_ALL_PATH "/GUI/WorldOverlay/")
    list (APPEND EPI_ALL_PATH "/GUI/TaskManager/")
    list (APPEND EPI_ALL_PATH "/GUI/Viewport/")
    list (APPEND EPI_ALL_PATH "/GUI/Viewport/Tools/")
    list (APPEND EPI_ALL_PATH "/ImportInfo/")

    #on configure QT
    AG_CONFIG_QT4(EPI_ALL_PATH)
    # Generating QRC's
    file(GLOB QRC_FILES resources.qrc)
    qt4_add_resources(QRC_FILES_CPP ${QRC_FILES})
    SOURCE_GROUP("_Generated" FILES ${QRC_FILES_CPP})
    
    #on regroupe l'ensemble des fichiers includes et sources
    ag_create_incFile_srcFile(EPI_ALL_PATH EPI_INC_FILES EPI_SRC_FILES)
    
    #on regroupe les fichers sources avec les moc_cpp
    set(EPI_ALL_SRC ${MOC_FILES_CPP})
    list (APPEND EPI_ALL_SRC ${QRC_FILES_CPP})
    list (APPEND EPI_ALL_SRC ${EPI_SRC_FILES})

    #on configure le precompiled header
    AG_MAKE_PRECOMPILED_HEADERS(EPI EPI_ALL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

    add_library (EPI ${EPI_INC_FILES} ${EPI_ALL_SRC})
    target_link_libraries (EPI ${QT_LIBRARIES} Core EPI Universe RendererSM2 GfxDx9 Workflow Packing QtToolbox AudioOpenAL)
ELSE  (QT_WRAP_CPP AND QT_WRAP_UI)
    MESSAGE(STATUS "EPI : QT Not found... project skipped.")
ENDIF (QT_WRAP_CPP AND QT_WRAP_UI)

AG_PROJECT_END(EPI Nyx)