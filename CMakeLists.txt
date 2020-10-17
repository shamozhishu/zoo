###########################################################################
#                                                                         #
#                               zoo solution                              #
#                                                                         #
###########################################################################
project(zoo)

cmake_minimum_required(VERSION 3.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

#设置生成动态库和静态库
if (WIN32)
    SET(BUILD_SHARED_LIBS "On")
else(UNIX)
    SET(BUILD_SHARED_LIBS "On")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")
    SET(CMAKE_VERBOSE_MAKEFILE ON)
endif()

#设置调试库的后缀
SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")

#设置库和可执行文件的输出文件
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

SET(SOLUTION_INCLUDE ${CMAKE_SOURCE_DIR}/include)
INSTALL(DIRECTORY ${SOLUTION_INCLUDE}/ DESTINATION include)

#包含三方库

#Qt
FIND_PACKAGE(Qt5Core)
FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5Gui)
FIND_PACKAGE(Qt5OpenGL)
FIND_PACKAGE(Qt5Network)
FIND_PACKAGE(Qt5Test)
FIND_PACKAGE(Qt5Sql)
FIND_PACKAGE(Qt5Concurrent)
FIND_PACKAGE(Qt5Xml)
IF ( Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5OpenGL_FOUND AND Qt5Network_FOUND AND Qt5Test_FOUND AND Qt5Sql_FOUND AND Qt5Concurrent_FOUND AND Qt5Xml_FOUND)
    message(STATUS "zoo use QT5")
    SET(QT_INCLUDES ${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Test_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS} ${Qt5Concurrent_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})
ENDIF ()

include(OsgEarthMacroUtils)
IF(WIN32 AND NOT ANDROID)
    IF(MSVC)
        OPTION(WIN32_USE_MP "Set to ON to build osgEarth with the /MP option (Visual Studio 2005 and above)." OFF)
        MARK_AS_ADVANCED(WIN32_USE_MP)
        IF(WIN32_USE_MP)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        ENDIF(WIN32_USE_MP)
    ENDIF(MSVC)
	SET(3RD_PARTY_DIR "" CACHE PATH "Set to base lib install path")
	SET(OSG_DIR "" CACHE PATH "Set to base OpenSceneGraph install path")
	SET(OSGEARTH_DIR "" CACHE PATH "Set to OSGEARTH install path")
	SET(DX_SDK_INCLUDE "" CACHE PATH "Set to DirectX lib install path")
	SET(DX_SDK_LIB "" CACHE PATH "Set to DirectX lib install path")
else()
	SET(3RD_PARTY_DIR "" CACHE PATH "Set to base lib install path")
	SET(OSG_DIR "" CACHE PATH "Set to base OpenSceneGraph install path")
	SET(OSGEARTH_DIR "" CACHE PATH "Set to OSGEARTH   install path")
	SET(DX_SDK_INCLUDE "" CACHE PATH "Set to DirectX lib install path")
	SET(DX_SDK_LIB "" CACHE PATH "Set to DirectX lib install path")
endif()

#创建plugins文件夹
IF(WIN32)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Debug/plugins)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release/plugins)
ENDIF()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(applications)

set(PATH_DX_RUNTIME "" CACHE PATH "add PATH_DX_RUNTIME to environment variable PATH")
set(PATH_OSG_RUNTIME "" CACHE PATH "add PATH_OSG_RUNTIME to environment variable PATH")
set(PATH_OSGEARTH_RUNTIME "" CACHE PATH "add PATH_OSGEARTH_RUNTIME to environment variable PATH")
set(PATH_3RD_PARTY_RUNTIME "" CACHE PATH "add PATH_3RD_PARTY_RUNTIME to environment variable PATH")
set(PATH_QT5_RUNTIME "" CACHE PATH "add PATH_QT5_RUNTIME to environment variable PATH")
set(PATH_TOLUA_RUNTIME "" CACHE PATH "add PATH_TOLUA_RUNTIME to environment variable PATH")
set(PATH_FLEX_AND_BISON_RUNTIME "" CACHE PATH "add PATH_FLEX_AND_BISON_RUNTIME to environment variable PATH")
set(ENV{PATH} "${PATH_DX_RUNTIME};${PATH_OSG_RUNTIME};${PATH_OSGEARTH_RUNTIME};${PATH_3RD_PARTY_RUNTIME};${PATH_QT5_RUNTIME};${PATH_TOLUA_RUNTIME};${PATH_FLEX_AND_BISON_RUNTIME}")
