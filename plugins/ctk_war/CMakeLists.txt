###########################################################################
#                                                                         #
#                                  ctk_war                                #
#                                                                         #
###########################################################################
project(ctk_war)
set(OUT_NAME ctk_war)

SET(CMAKE_DEBUG_POSTFIX "")
FIND_DEPENDENCY(LUA lua.h "lua;tolua++" ${3RD_PARTY_DIR} "d" "")

set(PLUGIN_export_directive "ctk_war_EXPORT")

IF(QT_INCLUDES)
	include_directories(${QT_INCLUDES})
ENDIF()

IF(WIN32 AND NOT ANDROID)
	# CTK
	IF(3RD_PARTY_DIR)
		SET(CTK_DIR ${3RD_PARTY_DIR}/lib/ctk-0.1/CMake)
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
	
	FIND_PATH(DIRECTINPUT_INCLUDE_DIR dinput.h)
	FIND_LIBRARY(DIRECTINPUT_LIBRARY dinput8.lib)
	FIND_LIBRARY(DIRECTINPUT_GUID_LIBRARY dxguid.lib)
	include_directories(${DIRECTINPUT_INCLUDE_DIR})
else()
	FIND_PACKAGE(CTK)
	include_directories(
		${CTKCore_INCLUDE_DIRS}
		${CTKPluginFramework_INCLUDE_DIRS}
	)

	include_directories("${3RD_PARTY_DIR}/include")
	link_directories("${3RD_PARTY_DIR}/lib")
endif()

file(WRITE ${CMAKE_BINARY_DIR}/plugins/ctk_war/ToLuaWar.cpp)

set(PLUGIN_SRCS
	${CMAKE_SOURCE_DIR}/include/component/war/WarComponents.h
	Battlefield.h
	LuaScript.h
	WarActivator.h
	WarCommander.h
	WarServiceImpl.h
	WarSimulator.h
	LuaExportClass.h

	WarComponents.cpp
	Battlefield.cpp
	LuaScript.cpp
	WarActivator.cpp
	WarCommander.cpp
	WarServiceImpl.cpp
	WarSimulator.cpp
	LuaExportClass.cpp
	
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
	debug ${LUA_LIBRARY_DEBUG} optimized ${LUA_LIBRARY}
	debug libtolua++d optimized libtolua++
	${DIRECTINPUT_LIBRARY}
	${DIRECTINPUT_GUID_LIBRARY}
)

if (WIN32)
add_custom_command(TARGET ${PROJECT_NAME}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_CURRENT_SOURCE_DIR}/ToLuaWar.pkg" "${CMAKE_BINARY_DIR}/plugins/ctk_war"
	"${CMAKE_SOURCE_DIR}/include/component/war/WarComponents.h" "${CMAKE_BINARY_DIR}/plugins/ctk_war"
	"${CMAKE_CURRENT_SOURCE_DIR}/Battlefield.h" "${CMAKE_BINARY_DIR}/plugins/ctk_war"
	"${CMAKE_CURRENT_SOURCE_DIR}/LuaExportClass.h" "${CMAKE_BINARY_DIR}/plugins/ctk_war"
	COMMAND tolua++ -n ToLuaWar
	-o "${CMAKE_BINARY_DIR}/plugins/ctk_war/ToLuaWar.cpp" "${CMAKE_BINARY_DIR}/plugins/ctk_war/ToLuaWar.pkg"
)
add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${plugin_RUNTIME_output_dir}/$<CONFIGURATION>/lib${lib_name}.dll"
        "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/plugins"
)
endif()
