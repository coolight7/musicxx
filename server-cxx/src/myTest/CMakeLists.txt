CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

aux_source_directory(. DIR_myTest)
set(NAME_myTest myTest)

if (WIN32)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /utf-8 /wd4200")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /wd4200 /Zc:__cplusplus /std:c++20")
else ()
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -fPIC -pipe -std=gnu90")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -pipe -std=c++2a -fexceptions")
endif ()

add_executable(${NAME_myTest} ${DIR_myTest})
target_link_libraries(${NAME_myTest} myServer)
if (WIN32)
	target_compile_definitions(
		${NAME_myTest} PRIVATE
		strdup=_strdup
		strcasecmp=_stricmp
		strncasecmp=_strnicmp
	)
endif ()