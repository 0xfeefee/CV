##
## * Project build
##
##   This project currently only supports windows.
##

cmake_minimum_required(VERSION 3.20)

##
## Command line arguments:
set(PROJECT_NAME    	    "My_Project" CACHE STRING "Project name")
set(PROJECT_ENABLE_LOGS     ON 		     CACHE STRING "Enable project logs")
set(PROJECT_ENGINE_BACKEND  "Raylib"     CACHE STRING "Engine backend { Raylib, DirectX }")
set(PROJECT_ENGINE_FRONTEND "Lua"        CACHE STRING "Engine frontend { Lua, Wren }")
set(BUILD_TYPE 			    "Debug"      CACHE STRING "Build type { Debug, Release }")
set(BUILD_ENABLE_LOGS       ON 		     CACHE STRING "Enable build logs")

## Create the project:
project(${PROJECT_NAME}
	VERSION 0.9.0
	LANGUAGES C CXX
)

## Switches:
set(BUILD_TYPE__DEBUG       	 OFF)
set(BUILD_TYPE__RELEASE     	 OFF)
set(BUILD_PLATFORM__WIN64 		 OFF)
set(BUILD_PLATFORM__LINUX64 	 OFF)
set(BUILD_FRONTEND_LUA           OFF)
set(BUILD_FRONTEND_WREN          OFF)
set(BUILD_BACKEND_RAYLIB         OFF)
set(BUILD_BACKEND_DIRECTX        OFF)

## Build state:
set(SRC) 				      ## Directory containing project source code.
set(VENDOR) 			      ## Directory containing 3rd party libraries.
set(BUILD_PLATFORM) 	      ## { WIN64, LINUX64 }
set(BUILD_EXECUTABLE_NAME)    ## Name for the executable.
set(PROJECT_SOURCES)          ## Project source files.
set(PROJECT_INCLUDE_DIRS)     ## Include directories for project source files.
set(PROJECT_VERSION_STRING)   ## Current version in string format.
set(PROJECT_INFO_STRING)      ## Project name, build type and version in string format.
set(PROJECT_INFO_STRING_FULL) ## Same as { PROJECT_INFO_STRING } but with added platform and compiler info.

## Set CMake state:
set(CMAKE_BUILD_TYPE   ${BUILD_TYPE})

## Backend selection:
if (PROJECT_ENGINE_BACKEND STREQUAL "Raylib")
	set(BUILD_BACKEND_RAYLIB  ON)
	add_compile_definitions(PROJECT_ENGINE_BACKEND_RAYLIB=1)
elseif (PROJECT_ENGINE_BACKEND STREQUAL "DirectX")
	set(BUILD_BACKEND_DIRECTX ON)
	add_compile_definitions(PROJECT_ENGINE_BACKEND_DIRECTX=1)
else()
	message(FATAL_ERROR "Unknown backend selected: ${PROJECT_ENGINE_BACKEND}")
endif()

## Frontend selection:
if (PROJECT_ENGINE_FRONTEND STREQUAL "Lua")
	set(BUILD_FRONTEND_LUA  ON)
elseif(PROJECT_ENGINE_FRONTEND STREQUAL "Wren")
	set(BUILD_FRONTEND_WREN ON)
else()
	message(FATAL_ERROR "Unknown frontend selected: ${PROJECT_ENGINE_FRONTEND}")
endif()

## Log verbosity:
if (BUILD_ENABLE_LOGS)
	set(CMAKE_MESSAGE_LOG_LEVEL "VERBOSE")
else()
	set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")
endif()

## C++ compiler configuration:
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_COMPILER 		"cl")
	set(CMAKE_CXX_FLAGS_DEBUG 	"/Od /WX /EHs")
	set(CMAKE_CXX_FLAGS_RELEASE "/O3 /EHs")
	set(CMAKE_C_COMPILER        "cl")
elseif (CMAKE_CXX_COMPILER STREQUAL "GCC")
	set(CMAKE_CXX_COMPILER 	  	"g++")
	set(CMAKE_CXX_FLAGS_DEBUG 	"-O0 -g -Wall")
	set(CMAKE_CXX_FLAGS_RELEASE "/O3")
	set(CMAKE_C_COMPILER        "gcc")
else()
	message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}, valid options are: { MSVC, GCC }")
endif()

set(BUILD_COMPILER ${CMAKE_CXX_COMPILER_ID})

## Basic config
set(CMAKE_CXX_STANDARD     17)
set(SRC 	 	   		   "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(VENDOR	   			   "${CMAKE_CURRENT_SOURCE_DIR}/../vendor")
set(BUILD_EXECUTABLE_NAME  "${PROJECT_NAME}")

## Build type:
if (BUILD_TYPE STREQUAL "Release")
	set(BUILD_TYPE__RELEASE ON)
	add_compile_definitions(PROJECT_BUILD_RELEASE=1)
elseif (BUILD_TYPE STREQUAL "Debug")
	set(BUILD_TYPE__DEBUG     ON)
	set(BUILD_EXECUTABLE_NAME "${BUILD_EXECUTABLE_NAME}__Debug")
	add_compile_definitions(PROJECT_BUILD_DEBUG=1)
else()
	message(FATAL_ERROR "Invalid BUILD_TYPE given, valid options are: { Debug, Release }.")
endif()

## Check if we are on 64 bit platform:
if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(FATAL_ERROR "Only 64 bit platforms are supported!")
endif()

## Platform specific variables:
if (WIN32)
	add_compile_definitions(
		PROJECT_PLATFORM_WIN64=1
		WIN32_LEAN_AND_MEAN=1
		NOMINMAX=1
	)
	set(VENDOR "${VENDOR}/Win64")
	set(BUILD_PLATFORM "WIN64")
	set(BUILD_PLATFORM__WIN64 ON)
elseif (UNIX AND NOT APPLE)
	add_compile_definitions(PROJECT_PLATFORM_LINUX64=1)
	set(VENDOR "${VENDOR}/Linux64")
	set(BUILD_PLATFORM "LINUX64")
	set(BUILD_PLATFORM__LINUX64 ON)
else()
	message(FATAL_ERROR "Unsupported platform!")
endif()

## Project metadata strings:
set(PROJECT_VERSION_STRING   "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_INFO_STRING      "${CMAKE_PROJECT_NAME} v${PROJECT_VERSION_STRING} (${BUILD_TYPE})")
set(PROJECT_INFO_STRING_FULL "${PROJECT_INFO_STRING} [${BUILD_PLATFORM}, ${BUILD_COMPILER}]")

## Metadata definitions:
add_compile_definitions(
	PROJECT_NAME="${CMAKE_PROJECT_NAME}"
	PROJECT_VERSION_STRING="${PROJECT_VERSION_STRING}"
	PROJECT_INFO_STRING="${PROJECT_INFO_STRING}"
	PROJECT_INFO_STRING_FULL="${PROJECT_INFO_STRING_FULL}"
	PROJECT_ENGINE_BACKEND="${PROJECT_ENGINE_BACKEND}"
	PROJECT_ENGINE_FRONTEND="${PROJECT_ENGINE_FRONTEND}"
)

## Enable logging utilities:
if (PROJECT_ENABLE_LOGS)
	add_compile_definitions(PROJECT_ENABLE_LOGS=1)
endif()

## Vendor variables:
set(VENDOR_INCLUDE_DIRS)
set(VENDOR_LIBRARIES)

## Add: backend
if (BUILD_BACKEND_RAYLIB)
	list(APPEND VENDOR_INCLUDE_DIRS
		"${VENDOR}/raylib-v5.0/include"
	)

	list(APPEND VENDOR_LIBRARIES
		"${VENDOR}/raylib-v5.0/lib/raylib.lib"
		winmm
	)
endif()

if (BUILD_BACKEND_DIRECTX)
	list(APPEND VENDOR_LIBRARIES
		d3d11
		dxgi
		d3dcompiler
	)
endif()

## Add: frontend
if (BUILD_FRONTEND_LUA)
	## Lua
	list(APPEND VENDOR_INCLUDE_DIRS
		"${VENDOR}/lua-v5.4.2/include"
		"${VENDOR}/sol2-v3.3.0/include"
	)

	list(APPEND VENDOR_LIBRARIES
		"${VENDOR}/lua-v5.4.2/lib/lua54.lib"
	)
elseif (BUILD_FRONTEND_WREN)
	message(FATAL_ERROR "Wren frontend not ready yet!")
endif()

## Add: spdlog
list(APPEND VENDOR_INCLUDE_DIRS
	"${VENDOR}/spdlog-v1.14.1/include"
)

##
## Create an executable:
set(SOURCE_FILES
	${SRC}/main.cpp
	${SRC}/base.pch.cpp
	${SRC}/ecs/Entity.cpp
	${SRC}/ecs/Component.cpp
	${SRC}/ecs/System.cpp
	${SRC}/ecs/Registry.cpp
	${SRC}/2d_engine/2d_engine.cpp
)

## Add the current backend and frontend files:
file(GLOB_RECURSE _PROJECT_BACKEND_FILES "${SRC}/2d_engine/backends/${PROJECT_ENGINE_BACKEND}/*.cpp")
file(GLOB_RECURSE _PROJECT_FRONTEND_FILES "${SRC}/2d_engine/frontends/${PROJECT_ENGINE_FRONTEND}/*.cpp")

list(APPEND SOURCE_FILES ${_PROJECT_BACKEND_FILES} ${_PROJECT_FRONTEND_FILES})

add_executable(${BUILD_EXECUTABLE_NAME}
	${SOURCE_FILES}
)

target_include_directories(${BUILD_EXECUTABLE_NAME}
	PUBLIC
	${SRC}
	${VENDOR_INCLUDE_DIRS}
)

target_precompile_headers(${BUILD_EXECUTABLE_NAME}
	PRIVATE
	${SRC}/base.pch.hpp
)

if (BUILD_BACKEND_DIRECTX)
	target_link_options(${BUILD_EXECUTABLE_NAME}
		PRIVATE
		/ENTRY:WinMainCRTStartup
	)
endif()

target_link_libraries(${BUILD_EXECUTABLE_NAME}
	PRIVATE
	${VENDOR_LIBRARIES}
)


##
## Log build variables (will log nothing if BUILD_LOGS are disabled)
message(NOTICE "[BUILD - vars]")
message(NOTICE " * PROJECT_ENGINE_BACKEND: ${PROJECT_ENGINE_BACKEND}")
message(NOTICE " * PROJECT_ENGINE_FRONTEND: ${PROJECT_ENGINE_FRONTEND}")
message(NOTICE " * BUILD_TYPE: ${BUILD_TYPE}")
message(NOTICE " * BUILD_COMPILER: ${BUILD_COMPILER}")
message(NOTICE " * BUILD_EXECUTABLE_NAME: ${BUILD_EXECUTABLE_NAME}")
message(NOTICE " * PROJECT_INFO_STRING_FULL: ${PROJECT_INFO_STRING_FULL}")
message(NOTICE " * VENDOR: ${VENDOR}")
message(NOTICE " * SRC: ${SRC}")

message(NOTICE " * PROJECT_SOURCES:")
foreach(source_file ${SOURCE_FILES})
	message(NOTICE "   - ${source_file}")
endforeach()

message(NOTICE " * VENDOR_INCLUDE_DIRS:")
foreach(include_dir ${VENDOR_INCLUDE_DIRS})
	message(NOTICE "  - ${include_dir}")
endforeach()

message(NOTICE " * VENDOR_LIBRARIES:")
foreach(lib ${VENDOR_LIBRARIES})
	message(NOTICE " - ${lib}")
endforeach()
