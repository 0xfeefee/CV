##
## * Project build
##
##   This project currently only supports windows.
##

cmake_minimum_required(VERSION 3.20)

## Command line arguments:
set(PROJECT_NAME    	  "My_Project" CACHE STRING "Project name")
set(PROJECT_ENABLE_LOGS   ON 		   CACHE STRING "Enable project logs")
set(BUILD_TYPE 			  "Debug"      CACHE STRING "Build type { Debug, Release }")
set(BUILD_COMPILER        "Clang"      CACHE STRING "Compiler { MSVC, Clang, GCC }")
set(BUILD_ENABLE_LOGS     ON 		   CACHE STRING "Enable build logs")

## Switches:
set(BUILD_TYPE__DEBUG       OFF)
set(BUILD_TYPE__RELEASE     OFF)
set(BUILD_PLATFORM__WIN64 	OFF)
set(BUILD_PLATFORM__LINUX64 OFF)

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

## C++ compiler configuration:
if (BUILD_COMPILER STREQUAL "MSVC")
	set(CMAKE_CXX_COMPILER 		"cl")
	set(CMAKE_CXX_FLAGS_DEBUG 	"/Od /WX /EHs")
	set(CMAKE_CXX_FLAGS_RELEASE "/O3 /EHs")
	set(CMAKE_C_COMPILER        "cl")
elseif (BUILD_COMPILER STREQUAL "Clang")
	set(CMAKE_CXX_COMPILER 		"clang++")
	set(CMAKE_CXX_FLAGS_DEBUG   "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_C_COMPILER        "clang")
elseif (BUILD_COMPILER STREQUAL "GCC")
	set(CMAKE_CXX_COMPILER 	  	"g++")
	set(CMAKE_CXX_FLAGS_DEBUG 	"-O0 -g -Wall")
	set(CMAKE_CXX_FLAGS_RELEASE "/O3")
	set(CMAKE_C_COMPILER        "gcc")
else()
	message(FATAL_ERORR "Unsupported compiler: ${BUILD_COMPILER}, valid options are: { MSVC, Clang, GCC }")
endif()

## Log verbosity:
if (BUILD_ENABLE_LOGS)
	set(CMAKE_MESSAGE_LOG_LEVEL "VERBOSE")
else()
	set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")
endif()


##
## Create the project:
project(${PROJECT_NAME}
	VERSION 0.2.0
	LANGUAGES C CXX
)


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
	set(BUILD_TYPE__DEBUG   ON)
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
	add_compile_definitions(PROJECT_PLATFORM_WIN64=1)
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
)

if (PROJECT_ENABLE_LOGS)
	add_compile_definitions(PROJECT_ENABLE_LOGS=1)
endif()

## Packages:
find_package(OpenGL REQUIRED)

## 3rd party:
set(VENDOR_INCLUDE_DIRS
	"${VENDOR}/spdlog-v1.14.1/include"
)


##
## Create an executable:
set(SOURCE_FILES
	${SRC}/main.cpp
	${SRC}/base.pch.cpp
)

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


##
## Log build variables (will log nothing if BUILD_LOGS are disabled)
message(NOTICE "[BUILD - vars]\n * SRC: ${SRC}")
message(NOTICE " * VENDOR: ${VENDOR}")
message(NOTICE " * BUILD_TYPE: ${BUILD_TYPE}")
message(NOTICE " * BUILD_COMPILER: ${BUILD_COMPILER}")
message(NOTICE " * BUILD_EXECUTABLE_NAME: ${BUILD_EXECUTABLE_NAME}")
message(NOTICE " * PROJECT_INFO_STRING_FULL: ${PROJECT_INFO_STRING_FULL}")
message(NOTICE " * PROJECT_SOURCES:")
foreach(source_file ${SOURCE_FILES})
	message(NOTICE "   - ${source_file}")
endforeach()
message(NOTICE " * VENDOR_INCLUDE_DIRS:")
foreach(include_dir ${VENDOR_INCLUDE_DIRS})
	message(NOTICE "  - ${include_dir}")
endforeach()
