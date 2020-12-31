###########################################################################
#                                                                         #
#                                zooCmdUI                                 #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmdUI)

set(ZOOCMDUI_CPP
	main.cpp
	resource.h
	ZooCmdUI.rc
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

IF(QT_INCLUDES)
	include_directories(${QT_INCLUDES})
ENDIF()

add_executable(
	${OUT_NAME}
	${ZOOCMDUI_CPP}
)

Set_Target_Properties(${OUT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
Set_Target_Properties(${OUT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

target_link_libraries(
	${OUT_NAME} 
	${Qt5Core_LIBRARIES}
	${Qt5Widgets_LIBRARIES}
)

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
