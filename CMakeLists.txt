cmake_minimum_required(VERSION 2.6.2)

#-----------------------------------------------------------------------------
# Ces options de compilations sont appliquees sur l'ensemble des projets,
# sauf indication contraires.
#-----------------------------------------------------------------------------
#option(ENABLE_PRECOMPILED_HEADER "enable precompiled header" OFF) #ON or OFF
if(WIN32 OR WIN64)
set(ENABLE_PRECOMPILED_HEADER "ON") #ON or OFF
else()
set(ENABLE_PRECOMPILED_HEADER "OFF") #ON or OFF
endif(WIN32 OR WIN64)

set(PRELUDE_H Sources/Prelude.h)
set(PRELUDE_H_ABS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${PRELUDE_H})



#-----------------------------------------------------------------------------
# On determine le compilateur utilise (Permet d'initialiser certaine variables de CMake (CMAKE_COMPILER_IS_GNUCXX, ...)
#-----------------------------------------------------------------------------
include (${CMAKE_ROOT}/Modules/CMakeDetermineCCompiler.cmake)
include (${CMAKE_ROOT}/Modules/CMakeDetermineCXXCompiler.cmake)

#-----------------------------------------------------------------------------
# On determine les configurations utilisees : Debug, Release, ...
#-----------------------------------------------------------------------------
IF(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -pthread -msse -D__SSE__")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Winline --param large-function-growth=400")
ELSE()
    set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /Zp16")
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# flag ReleaseDev
set(CMAKE_CXX_FLAGS_RELEASEDEV "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASEDEV "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASEDEV "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;ReleaseDev" CACHE STRING "Configs" FORCE)

# DevMode

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG LM_DEVMODE)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASEDEV LM_DEVMODE LM_RELEASE_DEV)

include(CMakeAGTools.cmake)


#-----------------------------------------------------------------------------
# On affiche les options donnees a CMake
#-----------------------------------------------------------------------------
get_filename_component(COMPILER_C_NAME ${CMAKE_C_COMPILER} NAME)
get_filename_component(COMPILER_CXX_NAME ${CMAKE_CXX_COMPILER} NAME)
message( STATUS )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "Compilateur C: ${COMPILER_C_NAME}")
message( STATUS "Compilateur C++: ${COMPILER_CXX_NAME}")
message( STATUS "" )
message( STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
message( STATUS "ENABLE_PRECOMPILED_HEADER = ${ENABLE_PRECOMPILED_HEADER}" )
message( STATUS "CMAKE_CONFIGURATION_TYPES = ${CMAKE_CONFIGURATION_TYPES}" )
message( STATUS "" )
message( STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}" )
message( STATUS "CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}" )
message( STATUS "CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}" )
message( STATUS "CMAKE_SHARED_LINKER_FLAGS_DEBUG = ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}" )
message( STATUS "CMAKE_SHARED_LINKER_FLAGS_RELEASE = ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS )

#-----------------------------------------------------------------------------
# On definie les repertoires de sortie
#-----------------------------------------------------------------------------
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/../OutputExe)
IF(BUILD_SHARED_LIBS)
    set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
ELSE(BUILD_SHARED_LIBS)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../OutputLibs)
ENDIF(BUILD_SHARED_LIBS)

#-----------------------------------------------------------------------------
# Gestion du type de compilation (Build:DEBUG/RELEASE)
#-----------------------------------------------------------------------------
#IF(NOT CMAKE_BUILD_TYPE)
#    MESSAGE(FATAL_ERROR "La configuration n'a pas ete specifiee : choix possible DEBUG/RELEASE")
#ELSE()
#    IF(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
#        ADD_DEFINITIONS("-D_DEBUG")
#    ELSEIF(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
#    ELSE(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
#        MESSAGE(FATAL_ERROR "La configuration n'a pas ete identifiee : choix possible DEBUG/RELEASE")
#    ENDIF(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
#ENDIF(NOT CMAKE_BUILD_TYPE)

#-----------------------------------------------------------------------------
#le define comptenant le nom du module utilisé par le log
#-----------------------------------------------------------------------------
#ADD_DEFINITIONS(-DLM_LOG_MODULE_NAME="module a definir")

#-----------------------------------------------------------------------------
# Gestion des options du compilateur et de l'OS
#-----------------------------------------------------------------------------
IF(CMAKE_COMPILER_IS_GNUCXX)
    link_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Librairies/MinGW_GCC/)
ELSE(CMAKE_COMPILER_IS_GNUCXX)
    link_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Librairies/VC90/Win32)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET (AG_WORKING_DIR L"${CMAKE_CURRENT_SOURCE_DIR}")

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/Sources/Modules)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/Sources/Tools)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/Sources/HOO)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/Sources/ChronoRage)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes/VS/DirectX)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes/lua)

# Pour OpenCollada

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes/COLLADABaseUtils)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes/COLLADAFramework)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes/COLLADASaxFrameworkLoader)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes/GeneratedSaxParser)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes/LibXML)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes/MathMLSolver)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/LibsExternes/Includes/pcre)


# Pour les headers précompilés
#include_directories (${CMAKE_CURRENT_BINARY_DIR})

#-----------------------------------------------------------------------------
#Sous Windows on place l'API en export
#-----------------------------------------------------------------------------
#IF(BUILD_SHARED_LIBS)
#    IF(WIN32 OR WIN64)
#        ADD_DEFINITIONS(-DLM_API=__declspec\(dllexport\))
#    ELSE(WIN32 OR WIN64)
#        ADD_DEFINITIONS(-DLM_API= )
#    ENDIF(WIN32 OR WIN64)
#ELSE(BUILD_SHARED_LIBS)
#    ADD_DEFINITIONS(-DLM_API= )
#ENDIF(BUILD_SHARED_LIBS)

#-----------------------------------------------------------------------------
# Macro
#-----------------------------------------------------------------------------


MACRO(AG_PROJECT AG_PROJECT_NAME AG_PROJECT_SHORT_NAME)
project (${AG_PROJECT_NAME})
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} ASS)#Assets
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} AUD)#Audio
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} OAL)#AudioOpenAL
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} ADS)#AudioStub
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} COR)#Core
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} DFT)#Draft
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} GFX)#Gfx
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} GX9)#GfxDx9
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} GUI)#GUI
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} IMG)#Images
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} NET)#Network
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} PAK)#Packing
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} QTB)#QtToolbox
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} RDR)#Renderer
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} RDS)#RendererStub
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TUA)#TUAssets
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TUC)#TUCore
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TUG)#TUGameEngine
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TUN)#TUNetwork
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TUT)#TUTools
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TUU)#TUUniverse
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TUS)#TUSocial
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TUW)#TUWeb
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TVT)#TVTools
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TVA)#TVAudioOpenAL
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TVG)#TVGfxDx9
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TVI)#TVGUI
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TVR)#TVRenderer
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TVU)#TVUniverse
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} SM2)#RendererSM2
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} UNI)#Universe
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} WIN)#Window
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} WKF)#WorkFlow
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} EPI)#EPI
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} NYX)#Nyx
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TVC)#TVNetworkClient
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} TVS)#TVNetworkServer
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} HOS)#HOOServer
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} HOC)#HOOClient
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} HCB)#HOOClientBot
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} HCS)#HOOClientStub
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} HAP)#HOOAdminProtocol
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} HAT)#HOOAdminTool
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} HUP)#HOOUserProtocol
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} HUDP)#HOOUpdaterProtocol
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} HGE)#HOOGameEngine
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} CHR)#ChronoRage
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} SCL)#Social
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} SCP)#ShaderCompiler
AG_MODULE_IMPORT_EXPORT(${AG_PROJECT_SHORT_NAME} WEB)#Web
REMOVE_DEFINITIONS(-DLM_LOG_MODULE_NAME)
ADD_DEFINITIONS(-DLM_LOG_MODULE_NAME=L"${AG_PROJECT_SHORT_NAME}")

ADD_DEFINITIONS(-DLM_WORKING_DIRECTORY=${AG_WORKING_DIR})
ENDMACRO(AG_PROJECT)

SET(INSTALL_BINDIR "bin")
MACRO(AG_PROJECT_END AG_PROJECT_NAME PROJECT_COMPONENT)
INSTALL(TARGETS ${AG_PROJECT_NAME} DESTINATION ${INSTALL_BINDIR} COMPONENT ${PROJECT_COMPONENT} CONFIGURATIONS ReleaseDev)
ENDMACRO(AG_PROJECT_END)

INSTALL(DIRECTORY "D:/Work/Dev/LibsExternes/Dll/VC90/Win32/" DESTINATION "bin"  COMPONENT LibsExternes FILES_MATCHING PATTERN "*.dll")
#-----------------------------------------------------------------------------
# Gestion des headers précompilés
#-----------------------------------------------------------------------------
#set(PRELUDE_H      ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Prelude.h)
#set(PRELUDE_COPY_H ${CMAKE_CURRENT_BINARY_DIR}/Prelude.h)
#set(PRELUDE_PCH    ${CMAKE_CURRENT_BINARY_DIR}/Prelude.h.gch)

MACRO(USE_PRECOMPILED_HEADER targetName)
#    SET_TARGET_PROPERTIES(${targetName} PROPERTIES COMPILE_FLAGS "-include ${PRELUDE_COPY_H} -Winvalid-pch")
#    ADD_DEPENDENCIES(${targetName} PrecompiledHeader)           
ENDMACRO(USE_PRECOMPILED_HEADER)

#STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
#SET(_compile_FLAGS ${${_flags_var_name}})
   
#GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)

#FOREACH(item ${_directory_flags})
#    LIST(APPEND _compile_FLAGS "-I${item}")
#ENDFOREACH(item)
  
#GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
#LIST(APPEND _compile_FLAGS ${_directory_flags})
#LIST(APPEND _compile_FLAGS ${CMAKE_CXX_FLAGS} )

#SEPARATE_ARGUMENTS(_compile_FLAGS)
       
#add_custom_command(
#    OUTPUT  ${PRELUDE_COPY_H}
#    COMMAND ${CMAKE_COMMAND} -E copy  ${PRELUDE_H} ${PRELUDE_COPY_H}
#    DEPENDS ${PRELUDE_H}
#)

#add_custom_command(
#    OUTPUT  ${PRELUDE_PCH}
#    COMMAND ${CMAKE_CXX_COMPILER} ${_compile_FLAGS} -x c++-header -o ${PRELUDE_PCH} ${PRELUDE_COPY_H}
#    DEPENDS ${PRELUDE_H} ${PRELUDE_COPY_H}
#)

#add_custom_target(PrecompiledHeader
#    DEPENDS ${PRELUDE_H} ${PRELUDE_PCH} ${PRELUDE_COPY_H}
#)

#-----------------------------------------------------------------------------
# Ce projet permet surtout de factoriser les options de compilation et de 
# pouvoir compiler tout d'une seule traite.
#-----------------------------------------------------------------------------
project (FULL)

#-----------------------------------------------------------------------------
# Liste des modules
#-----------------------------------------------------------------------------
add_subdirectory (Sources)
add_subdirectory (TestsUnitaires)
add_subdirectory (TestsValidation)

