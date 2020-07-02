###########################################################################
#                                                                         #
#                                zooCmdUI                                 #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmdUI)

set(PROJECT_SRC ${CMAKE_CURRENT_SOURCE_DIR})

set(ZOO_CPP
	${PROJECT_SRC}/main.cpp
)
           
add_executable(
	${OUT_NAME}
	${ZOO_CPP}
)

target_link_libraries(
	${OUT_NAME}
	zooCmdLoader
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

IF(QT_INCLUDES)
	include_directories(${QT_INCLUDES})
ENDIF()

IF(Qt5Widgets_FOUND)	
	target_link_libraries(
		${OUT_NAME} 
		${Qt5Core_LIBRARIES}
		${Qt5Widgets_LIBRARIES}
		zooCmdLoader
		)							
ELSE()
	target_link_libraries(
		${OUT_NAME} 
		${QT_LIBRARIES}
		zooCmdLoader
		)
ENDIF()

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
