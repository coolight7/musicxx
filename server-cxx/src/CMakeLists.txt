CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

find_package(OpenSSL REQUIRED)
if(WIN32)
	find_package(workflow REQUIRED CONFIG HINTS ${CMAKE_SOURCE_DIR}/mylib/workflow_win/)
else()
	find_package(workflow REQUIRED CONFIG HINTS ${CMAKE_SOURCE_DIR}/mylib/workflow_linux/)
endif()
include_directories(${OPENSSL_INCLUDE_DIR} ${WORKFLOW_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)

ADD_SUBDIRECTORY(myServer)
ADD_SUBDIRECTORY(myMusic)
ADD_SUBDIRECTORY(myTest)

