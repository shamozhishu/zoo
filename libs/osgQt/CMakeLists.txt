###########################################################################
#                                                                         #
#                                  osgQt                                  #
#                                                                         #
###########################################################################
set(OUT_NAME osgQt)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs)

include_directories("${3RD_PARTY_DIR}/include")
include_directories("${OSG_DIR}/include")

link_directories("${3RD_PARTY_DIR}/lib")
link_directories("${OSG_DIR}/lib")

include(Find3rdPartyDependencies)
ADD_DEFINITIONS(-DOSGQT_LIBRARY)

file(GLOB OSG_QT_H "*.h")
file(GLOB OSG_QT_CPP "*.cpp")

add_library(
    ${OUT_NAME} SHARED 
    ${OSG_QT_H}
    ${OSG_QT_CPP}
)

IF(QT_INCLUDES)
	include_directories(${QT_INCLUDES})
ENDIF()

target_link_libraries(
	${OUT_NAME}
	debug osgd optimized osg
	debug osgDBd optimized osgDB
	debug osgUtild optimized osgUtil
	debug osgViewerd optimized osgViewer
	debug osgGAd optimized osgGA
	debug OpenThreadsd optimized OpenThreads
	${Qt5Core_LIBRARIES}
	${Qt5Xml_LIBRARIES}
	${Qt5OpenGL_LIBRARIES}
	${Qt5Gui_LIBRARIES}
	${Qt5Widgets_LIBRARIES}
)

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
INSTALL(FILES ${ZOOCMD_H} DESTINATION include/osgQt)
