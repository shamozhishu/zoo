###########################################################################
#                                                                         #
#                             zooCmd_osg-earth                            #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmd_osg-earth)

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
	${PROJECT_INCLUDE}/EarthCmd.h
	${PROJECT_INCLUDE}/EarthControls.h
	${PROJECT_INCLUDE}/LonLatDistHandler.h
	${PROJECT_INCLUDE}/LocateModelHandler.h
	${PROJECT_INCLUDE}/MeasureDistHandler.h
)

set(ZOOCMD_PLUGIN_LIB_CPP
	${PROJECT_SRC}/EarthCmd.cpp
	${PROJECT_SRC}/EarthControls.cpp
	${PROJECT_SRC}/LonLatDistHandler.cpp
	${PROJECT_SRC}/LocateModelHandler.cpp
	${PROJECT_SRC}/MeasureDistHandler.cpp
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
