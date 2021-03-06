﻿###########################################################################
#                                                                         #
#                                  ctk_war                                #
#                                                                         #
###########################################################################
project(ctk_war)
set(OUT_NAME ctk_war)
set(CMAKE_DEBUG_POSTFIX "")

set(PLUGIN_export_directive "ctk_war_EXPORT")

IF(QT_INCLUDES)
	include_directories(${QT_INCLUDES})
ENDIF()
include_directories(${TOLUA_INCLUDE_DIR})
IF(WIN32 AND NOT ANDROID)
	# CTK
	IF(3RD_PARTY_DIR)
		FIND_PACKAGE(CTK REQUIRED)
		include_directories(
			${CTKCore_INCLUDE_DIRS}
			${CTKPluginFramework_INCLUDE_DIRS}
		)
		link_directories("${CTK_LIBRARY_DIR}")
	ELSE()
		message(FATAL_ERROR "3RD_PARTY_DIR need to set!!!")
	ENDIF()

	include_directories("${3RD_PARTY_DIR}/include")
	link_directories("${3RD_PARTY_DIR}/lib")
	include(Find3rdPartyDependencies)
else()
	FIND_PACKAGE(CTK)
	include_directories(
		${CTKCore_INCLUDE_DIRS}
		${CTKPluginFramework_INCLUDE_DIRS}
	)

	include_directories("${3RD_PARTY_DIR}/include")
	link_directories("${3RD_PARTY_DIR}/lib")
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/plugins/ctk_war/ToLuaWar.cpp")
	file(WRITE "${CMAKE_BINARY_DIR}/plugins/ctk_war/ToLuaWar.cpp")
endif()

file(GLOB CTK_PLUGIN_LIB_H "*.h")
file(GLOB CTK_PLUGIN_LIB_CPP "*.cpp")

set(PLUGIN_SRCS
	${CMAKE_SOURCE_DIR}/include/component/WarComponents.h
	${CTK_PLUGIN_LIB_H}
	${CTK_PLUGIN_LIB_CPP}
	ToLuaWar.cpp
	ToLuaWar.pkg
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
	WarActivator.h
	WarCommander.h
	WarServiceImpl.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_RESOURCES
)

#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
	NAME ${PROJECT_NAME}
	EXPORT_DIRECTIVE ${PLUGIN_export_directive}
	SRCS ${PLUGIN_SRCS}
	MOC_SRCS ${PLUGIN_MOC_SRCS}
	UI_FORMS ${PLUGIN_UI_FORMS}
	RESOURCES ${PLUGIN_RESOURCES}
	TARGET_LIBRARIES ${PLUGIN_target_libraries}
	${Qt5Core_LIBRARIES}
	zoo
	zooCmdLoader
	luaScript
	toluaMath
	toluaInput
)

if (WIN32)
add_custom_command(TARGET ${PROJECT_NAME}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_SOURCE_DIR}/include/component/WarComponents.h" "${CMAKE_BINARY_DIR}/plugins/ctk_war"
	"${CMAKE_CURRENT_SOURCE_DIR}/Battlefield.h" "${CMAKE_BINARY_DIR}/plugins/ctk_war"
	COMMAND tolua++ -n ToLuaWar
	-o "${CMAKE_BINARY_DIR}/plugins/ctk_war/ToLuaWar.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/ToLuaWar.pkg"
)
add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${plugin_RUNTIME_output_dir}/$<CONFIGURATION>/lib${lib_name}.dll"
        "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/plugins"
)
endif()

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib/$<CONFIGURATION>)
INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>/plugins)
