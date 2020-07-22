###########################################################################
#                                                                         #
#                              zooCmd_osg-world                           #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmd_osg-world)

set(PROJECT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_SRC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_INCLUDE})

IF(WIN32 AND NOT ANDROID)
	include_directories("${3RD_PARTY_DIR}/include")
	include_directories("${OSG_DIR}/include")
	include_directories("${OSGEARTH_DIR}/include")

	link_directories("${3RD_PARTY_DIR}/lib")
	link_directories("${OSG_DIR}/lib")
	link_directories("${OSGEARTH_DIR}/lib")
	include(Find3rdPartyDependencies)
else()
	include_directories("${3RD_PARTY_DIR}/include")
	include_directories("${OSG_DIR}/include")
	include_directories("${OSGEARTH_DIR}/include")

	link_directories("${3RD_PARTY_DIR}/lib")
	link_directories("${OSG_DIR}/lib")
	link_directories("${OSGEARTH_DIR}/lib")
endif()

set(ZOOCMD_PLUGIN_LIB_H 
	${PROJECT_INCLUDE}/LocateModelHandler.h
	${PROJECT_INCLUDE}/LongitudeLatitudeHandler.h
	${PROJECT_INCLUDE}/MeasureDistanceHandler.h
	${PROJECT_INCLUDE}/WorldCmd.h
	${PROJECT_INCLUDE}/WorldControls.h
)

set(ZOOCMD_PLUGIN_LIB_CPP
	${PROJECT_SRC}/LocateModelHandler.cpp
	${PROJECT_SRC}/LongitudeLatitudeHandler.cpp
	${PROJECT_SRC}/MeasureDistanceHandler.cpp
	${PROJECT_SRC}/WorldCmd.cpp
	${PROJECT_SRC}/WorldControls.cpp
)
    
add_library(
    	${OUT_NAME} SHARED 
    	${ZOOCMD_PLUGIN_LIB_H}
    	${ZOOCMD_PLUGIN_LIB_CPP}
)
ADD_DEFINITIONS(-DNEED_OSGEARTH_LIBRARY)
target_link_libraries(
		${OUT_NAME}
		zoo
		zooCmd
		zooCmd_osg
)

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
