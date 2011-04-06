
AG_PROJECT(CORE COR)

set (CORE_ALL_PATH "/" )
list (APPEND CORE_ALL_PATH "/Strings/")
list (APPEND CORE_ALL_PATH "/RT/")
list (APPEND CORE_ALL_PATH "/IO/")
list (APPEND CORE_ALL_PATH "/Memory/")
list (APPEND CORE_ALL_PATH "/Math/")
list (APPEND CORE_ALL_PATH "/Math/Splines/")
list (APPEND CORE_ALL_PATH "/Debug/")
list (APPEND CORE_ALL_PATH "/System/")
list (APPEND CORE_ALL_PATH "/VFS/")
list (APPEND CORE_ALL_PATH "/XML/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(CORE_ALL_PATH CORE_INC_FILES CORE_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(Core CORE_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

if(WIN32)
ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS) #pour supprimer les warnning 4996 du fichier DateAndTime
endif(WIN32)

add_library (Core ${CORE_INC_FILES} ${CORE_SRC_FILES})

if(WIN32 OR WIN64)
    target_link_libraries (Core winmm)
	target_link_libraries (Core debug lzma465_d)
	target_link_libraries (Core optimized lzma465)
else()
    target_link_libraries (Core boost_date_time-mt)
endif(WIN32 OR WIN64)

AG_PROJECT_END(Core Core)