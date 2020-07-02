###########################################################################
#                                                                         #
#                            ctk_pluginfwadmin                            #
#                                                                         #
###########################################################################
set(OUT_NAME ctk_pluginfwadmin)

SET(CMAKE_DEBUG_POSTFIX "")
set(PROJECT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_SRC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_INCLUDE})

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

set(QT_PLUGIN_LIB_H 
	${PROJECT_INCLUDE}/PluginFrameworkAdmin.h
)

set(QT_PLUGIN_LIB_CPP
	${PROJECT_SRC}/PluginFrameworkAdmin.cpp
)

set(LIB_MOC_HDRS
	${PROJECT_SRC}/PluginFrameworkAdmin.h
)

IF(Qt5Widgets_FOUND)
    SET(LIB_MOC_SRCS)
    FOREACH( LIB_MOC_HDR ${LIB_MOC_HDRS} )
        GET_FILENAME_COMPONENT( LIB_MOC_HDR_ABS ${LIB_MOC_HDR} ABSOLUTE )
        QT5_WRAP_CPP( LIB_MOC_SRC ${LIB_MOC_HDR} OPTIONS "-f${LIB_MOC_HDR_ABS}" )
        LIST( APPEND LIB_MOC_SRCS ${LIB_MOC_SRC} )
    ENDFOREACH()
ELSE()
    INCLUDE( ${QT_USE_FILE} )
    QT4_WRAP_CPP( LIB_MOC_SRCS ${LIB_MOC_HDRS} OPTIONS "-f" )
ENDIF()

add_library(
    	${OUT_NAME} SHARED 
    	${QT_PLUGIN_LIB_H}
    	${QT_PLUGIN_LIB_CPP}
		${LIB_MOC_HDRS}
		${LIB_MOC_SRCS}
)

IF(Qt5Widgets_FOUND)	
	target_link_libraries(
		${OUT_NAME} 
		${Qt5Core_LIBRARIES}
		zoo
		CTKPluginFramework
		)							
ELSE()
	target_link_libraries(
		${OUT_NAME} 
		${QT_LIBRARIES}
		zoo
		CTKPluginFramework
		)
ENDIF()



INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>/plugins)
