CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

aux_source_directory(. DIR_myServer)
set(NAME_myServer myServer)

#workflow + openssl
#find_library(LIBRT rt)

find_package(OpenSSL REQUIRED)
if(WIN32)
	find_package(workflow REQUIRED CONFIG HINTS ${CMAKE_SOURCE_DIR}/mylib/workflow_win/)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Release"
		OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
		find_library(LIBWORKFLOW_PATH workflow ${WORKFLOW_LIB_DIR}/Release)
	else()
		find_library(LIBWORKFLOW_PATH workflow ${WORKFLOW_LIB_DIR}/Debug)
	endif()
else()
	find_package(workflow REQUIRED CONFIG HINTS ${CMAKE_SOURCE_DIR}/mylib/workflow_linux/)
	find_library(LIBWORKFLOW_PATH workflow ${WORKFLOW_LIB_DIR})
endif()
include_directories(${OPENSSL_INCLUDE_DIR} ${WORKFLOW_INCLUDE_DIR})

#spdlog
find_package(spdlog REQUIRED)

#nlohmann_json
find_package(nlohmann_json CONFIG REQUIRED)

if (APPLE)
	set(WORKFLOW_LIB ${LIBWORKFLOW_PATH} pthread OpenSSL::SSL OpenSSL::Crypto)
elseif (WIN32)
	set(WORKFLOW_LIB ${LIBWORKFLOW_PATH} ws2_32 wsock32 OpenSSL::SSL OpenSSL::Crypto)
else ()
	#set(WORKFLOW_LIB workflow ${LIBRT} pthread OpenSSL::SSL OpenSSL::Crypto)
	set(WORKFLOW_LIB ${LIBWORKFLOW_PATH} pthread OpenSSL::SSL OpenSSL::Crypto)
endif ()

if (WIN32)
		# /wd4819 禁用msvc的4819[代码文件编码和当前系统环境不兼容]警告
		# 由于win中文系统默认为GB2312等非UTF-8编码，而项目整体代码都使用UTF-8，因此会有这个警告
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /utf-8 /wd4200")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /wd4200 /Zc:__cplusplus /std:c++20")
else ()
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -fPIC -pipe -std=gnu90")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -pipe -std=c++2a -fexceptions")
endif ()

add_library(${NAME_myServer} STATIC ${DIR_myServer})
target_link_libraries(${NAME_myServer}
	${WORKFLOW_LIB}
	fmt::fmt-header-only
	spdlog::spdlog_header_only 
	nlohmann_json
)
if (WIN32)
	target_compile_definitions(
		${NAME_myServer} PRIVATE
		strdup=_strdup
		strcasecmp=_stricmp
		strncasecmp=_strnicmp
	)
endif ()
