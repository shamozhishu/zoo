###########################################################################
#                                                                         #
#                               zooCmd_osg                                #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmd_osg)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

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

SET(PROJECT_INCLUDE ${CMAKE_SOURCE_DIR}/src/zooCmd_osg)
SET(PROJECT_SRC ${CMAKE_SOURCE_DIR}/src/zooCmd_osg)

set(ZOOCMD_OSG_H 
	${PROJECT_INCLUDE}/Support.h
	${PROJECT_INCLUDE}/InputDevice.h
	${PROJECT_INCLUDE}/SharedComponentImpls.h
)

set(ZOOCMD_OSG_CPP
	${PROJECT_SRC}/InputDevice.cpp
	${PROJECT_SRC}/SharedComponentImpls.cpp
)

if(${BUILD_SHARED_LIBS} STREQUAL "On")
    add_library(
    	${OUT_NAME} SHARED 
    	${ZOOCMD_OSG_H}
    	${ZOOCMD_OSG_CPP}
		)
    ADD_DEFINITIONS(-DZOOCMD_OSG_NONCLIENT_BUILD)
else()
    add_library(
    	${OUT_NAME} STATIC 
    	${ZOOCMD_OSG_H}
    	${ZOOCMD_OSG_CPP}
    	)
    ADD_DEFINITIONS(-DZOOCMD_OSG_NONCLIENT_BUILD_STATIC)
endif()

target_link_libraries(
	${OUT_NAME}
	zoo
	zooCmd
)
	
INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
INSTALL(FILES ${ZOOCMD_OSG_H} DESTINATION include/zooCmd_osg)
