###########################################################################
#                                                                         #
#                            ctk_pluginfwadmin                            #
#                                                                         #
###########################################################################
if (OPTION_CTK_PLUGINFWADMIN)

set(OUT_NAME ctk_pluginfwadmin)
set(CMAKE_DEBUG_POSTFIX "")

IF(QT_INCLUDES)
	include_directories(${QT_INCLUDES})
ENDIF()

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

file(GLOB QT_PLUGIN_LIB_H "*.h")
file(GLOB QT_PLUGIN_LIB_CPP "*.cpp")

set(LIB_MOC_HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/PluginFrameworkAdmin.h
)

IF(Qt5Widgets_FOUND)
    SET(LIB_MOC_SRCS)
    FOREACH( LIB_MOC_HDR ${LIB_MOC_HDRS} )
        GET_FILENAME_COMPONENT( LIB_MOC_HDR_ABS ${LIB_MOC_HDR} ABSOLUTE )
        QT5_WRAP_CPP( LIB_MOC_SRC ${LIB_MOC_HDR} OPTIONS "-f${LIB_MOC_HDR_ABS}" )
        LIST( APPEND LIB_MOC_SRCS ${LIB_MOC_SRC} )
    ENDFOREACH()
ELSE()
    message(FATAL_ERROR "Qt5Widgets_FOUND need to set!!!")
ENDIF()

add_library(
	${OUT_NAME} SHARED 
	${QT_PLUGIN_LIB_H}
	${QT_PLUGIN_LIB_CPP}
	${LIB_MOC_SRCS}
)

target_link_libraries(
	${OUT_NAME} 
	${Qt5Core_LIBRARIES}
	zoo
	CTKPluginFramework
)

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)

endif()
