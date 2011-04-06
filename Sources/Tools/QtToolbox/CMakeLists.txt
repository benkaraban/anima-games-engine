find_package(Qt4)

IF (QT_WRAP_CPP AND QT_WRAP_UI)
    AG_PROJECT(QTTOOLBOX QTB)
    
    # on regroupe les paths
    set (QTTOOLBOX_ALL_PATH "/" )
    list (APPEND QTTOOLBOX_ALL_PATH "/SpanSlider/")
    list (APPEND QTTOOLBOX_ALL_PATH "/ColorPicker/")
    list (APPEND QTTOOLBOX_ALL_PATH "/Slider2D/")
    
    #on configure QT
    AG_CONFIG_QT4(QTTOOLBOX_ALL_PATH)
    
    #on regroupe l'ensemble des fichiers includes et sources
    ag_create_incFile_srcFile(QTTOOLBOX_ALL_PATH QTTOOLBOX_INC_FILES QTTOOLBOX_SRC_FILES)
    
    #on regroupe les fichers sources avec les moc_cpp
    set(QTTOOLBOX_ALL_SRC ${MOC_FILES_CPP})
    list (APPEND QTTOOLBOX_ALL_SRC ${QTTOOLBOX_SRC_FILES})

    #on configure le precompiled header
    AG_MAKE_PRECOMPILED_HEADERS(QtToolbox QTTOOLBOX_ALL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

    add_library (QtToolbox ${QTTOOLBOX_INC_FILES} ${QTTOOLBOX_ALL_SRC})
    target_link_libraries(QtToolbox ${QT_LIBRARIES} Core Images)
ELSE  (QT_WRAP_CPP AND QT_WRAP_UI)
    MESSAGE(STATUS "QtToolbox : QT Not found... project skipped.")
ENDIF (QT_WRAP_CPP AND QT_WRAP_UI)

AG_PROJECT_END(QtToolbox QtToolbox)