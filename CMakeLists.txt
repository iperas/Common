CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# Common settings

CMAKE_POLICY(SET CMP0003 NEW)

MACRO(add_custom_action_copy targets)
	foreach(File ${targets})
	  add_custom_command(TARGET ${PROJECT} PRE_BUILD
	                     COMMAND ${CMAKE_COMMAND} -E
	                         copy ${File} $<TARGET_FILE_DIR:${PROJECT}>)
	endforeach()
ENDMACRO()

MACRO(add_test_ex target)
    ADD_TEST(NAME ${target} COMMAND $<TARGET_FILE:${target}> --gtest_output=xml:
${CMAKE_BINARY_DIR}/build/test-reports/${target}.xml)
ENDMACRO()

# Project settings

SET(PROJECT Common)
PROJECT(${PROJECT} CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

# Add some multithreaded build support
MARK_AS_ADVANCED(MULTITHREADED_BUILD)
set(MULTITHREADED_BUILD 12 CACHE STRING "How many threads are used to build the project")
if(MULTITHREADED_BUILD)
    if(CMAKE_COMPILER_IS_GNUCXX)
        message(STATUS ${CMAKE_BUILD_TOOL})
        set(CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM} -j${MULTITHREADED_BUILD}")
        message(STATUS "Added arguments to CMAKE_BUILD_TOOL: ${CMAKE_MAKE_PROGRAM}")
    elseif(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        message(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    endif()
endif()

# Libraries

## Qt

# (Make sure that CMAKE_PREFIX_PATH env var is set)
FIND_PACKAGE(Qt5Core)
FIND_PACKAGE(Qt5Sql)

## Boost

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost COMPONENTS regex system thread REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)

## Gtest

INCLUDE(CMakeGTest.txt)

# Common

FILE(GLOB_RECURSE HEADERS "${PROJECT}/*.h")

FILE(GLOB_RECURSE SOURCES "src/*.cpp")

FILE(GLOB_RECURSE MOC_SOURCES "*_automoc.cpp")

# source grouping

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})
source_group ("Generated Files" FILES ${MOC_SOURCES})


# includes

foreach(DEPENDENCY ${DEPENDS_ON_PROJECTS})
        include_directories("../${DEPENDENCY}")
endforeach()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}")

# linking

add_library(${PROJECT} STATIC ${HEADERS} ${SOURCES} ${MOC_SOURCES})
set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 14)
SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(
    ${PROJECT}
    ${DEPENDS_ON_PROJECTS}
    ${Boost_LIBRARIES}
    Qt5::Core
    debug ${GTEST_DEBUG_LIBS}
    optimized ${GTEST_RELEASE_LIBS}
    ${Pthread})

install ( TARGETS ${PROJECT} EXPORT ${PROJECT} ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/build/lib)
export ( TARGETS ${PROJECT} FILE ${CMAKE_BINARY_DIR}/${PROJECT}-config.cmake )
export ( PACKAGE ${PROJECT} )


# Tests

add_subdirectory(Common.Tests)
ENABLE_TESTING()
ADD_TEST_EX(Common.Tests)

