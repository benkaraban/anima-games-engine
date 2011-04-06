AG_PROJECT(WEB WEB)

set  (WEB_ALL_PATH "/" )
list (APPEND WEB_ALL_PATH "/jsoncpp/")
list (APPEND WEB_ALL_PATH "/Bittorrent/")

#on regroupe l'ensemble des fichiers includes et sources
ag_create_incFile_srcFile(WEB_ALL_PATH WEB_INC_FILES WEB_SRC_FILES)

AG_MAKE_PRECOMPILED_HEADERS(WEB WEB_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Prelude.h)

ADD_DEFINITIONS(-DTORRENT_USE_OPENSSL)
ADD_DEFINITIONS(-DTORRENT_DISABLE_GEO_IP)
ADD_DEFINITIONS(-DTORRENT_LINKING_SHARED)
ADD_DEFINITIONS(-DBOOST_ASIO_HASH_MAP_BUCKETS=1021)
ADD_DEFINITIONS(-D_SCL_SECURE_NO_DEPRECATE)
ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)
ADD_DEFINITIONS(-DBOOST_DISABLE_EXCEPTION)
ADD_DEFINITIONS(-DBOOST_ASIO_DISABLE_BUFFER_DEBUGGING)
ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)

add_library (Web ${WEB_INC_FILES} ${WEB_SRC_FILES})
    
IF(WIN32 OR WIN64)
    target_link_libraries (Web Core libcurl_imp)
    target_link_libraries (Web debug libtorrent_d)
    target_link_libraries (Web optimized libtorrent)
ELSE()
#Linux : Il faut installer libcurl4-openssl-dev pour pouvoir linker avec la lib curl
    target_link_libraries (Web Core curl)
# Pas besoin de torrent que pour les clients (Windows donc)
#    target_link_libraries (Web debug torrent_d)
#    target_link_libraries (Web optimized torrent)
ENDIF(WIN32 OR WIN64)
