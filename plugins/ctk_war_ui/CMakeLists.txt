###########################################################################
#                                                                         #
#                                 ctk_war_ui                              #
#                                                                         #
###########################################################################
project(ctk_war_ui)

SET(CMAKE_DEBUG_POSTFIX "")
set(PLUGIN_export_directive "ctk_war_ui_EXPORT")

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
else()
	FIND_PACKAGE(CTK)
	include_directories(
		${CTKCore_INCLUDE_DIRS}
		${CTKPluginFramework_INCLUDE_DIRS}
	)

	include_directories("${3RD_PARTY_DIR}/include")
	link_directories("${3RD_PARTY_DIR}/lib")
endif()

set(PLUGIN_SRCS
	UIActivator.h
	ArmyListWgt.h
	MenuToolButtons.h
	ComPropertyBoard.h
	PropertyWgts.h
	OpenBattlefieldDlg.h
	
	UIActivator.cpp
	ArmyListWgt.cpp
	MenuToolButtons.cpp
	ComPropertyBoard.cpp
	PropertyWgts.cpp
	OpenBattlefieldDlg.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
	UIActivator.h
	ArmyListWgt.h
	ComPropertyBoard.h
	PropertyWgts.h
	OpenBattlefieldDlg.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
	ComListWgt.ui
	ArmyListWgt.ui
	OpenBattlefieldDlg.ui
	DoFPropertyWgt.ui
	ModelPropertyWgt.ui
	CameraPropertyWgt.ui
	EarthPropertyWgt.ui
	BehaviorPropertyWgt.ui
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_RESOURCES
	ctk_war_ui.qrc
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
	${Qt5Xml_LIBRARIES}
	${Qt5OpenGL_LIBRARIES}
	${Qt5Gui_LIBRARIES}
	${Qt5Widgets_LIBRARIES}
	zoo
	zooCmdLoader
)

IF (WIN32)
add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${plugin_RUNTIME_output_dir}/$<CONFIGURATION>/lib${lib_name}.dll"
        "${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/plugins"
)
ENDIF()
