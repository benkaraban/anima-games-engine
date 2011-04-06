AG_PROJECT(WORKFLOW WKF)

set (WORKFLOW_ALL_PATH "/" )
list (APPEND WORKFLOW_ALL_PATH "/COLLADA/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(WORKFLOW_ALL_PATH WORKFLOW_INC_FILES WORKFLOW_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Workflow WORKFLOW_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

add_library (Workflow ${WORKFLOW_INC_FILES} ${WORKFLOW_SRC_FILES})
IF(CMAKE_COMPILER_IS_MINGW)
    IF(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
        SET(libD3Dx9 libd3dx9d_41.a)
    ELSE(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
        SET(libD3Dx9 libd3dx9_41.a)
    ENDIF(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
ELSE()

ENDIF(CMAKE_COMPILER_IS_MINGW)

target_link_libraries (Workflow Core Assets Images)

IF(CMAKE_COMPILER_IS_MINGW)
    target_link_libraries (Workflow libnvtt.dll.a libd3d9.a ${libD3Dx9})
ELSE()
    target_link_libraries (Workflow Images nvtt d3d9)
    target_link_libraries (Workflow debug d3dx9d)
    target_link_libraries (Workflow optimized d3dx9)

    # Pour OpenCollada
    target_link_libraries (Workflow debug COLLADABaseUtils-d optimized COLLADABaseUtils)
    target_link_libraries (Workflow debug COLLADAFramework-d optimized COLLADAFramework)
    target_link_libraries (Workflow debug COLLADASaxFrameworkLoader-d optimized COLLADASaxFrameworkLoader)
    target_link_libraries (Workflow debug GeneratedSaxParser-d optimized GeneratedSaxParser)
    target_link_libraries (Workflow debug LibXML-d optimized LibXML)
    target_link_libraries (Workflow debug MathMLSolver-d optimized MathMLSolver)
    target_link_libraries (Workflow debug pcre-d optimized pcre)
   
ENDIF(CMAKE_COMPILER_IS_MINGW)

AG_PROJECT_END(Workflow Workflow)