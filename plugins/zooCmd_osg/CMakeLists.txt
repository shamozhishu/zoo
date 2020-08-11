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

SET(PROJECT_INCLUDE ${CMAKE_SOURCE_DIR}/plugins/zooCmd_osg)
SET(PROJECT_SRC ${CMAKE_SOURCE_DIR}/plugins/zooCmd_osg)

set(ZOOCMD_OSG_H
	${CMAKE_SOURCE_DIR}/include/zooCmd_osg/OsgDevice.h
	${CMAKE_SOURCE_DIR}/include/zooCmd_osg/OsgEarthUtils.h
	${CMAKE_SOURCE_DIR}/include/zooCmd_osg/OsgEarthContext.h
	${CMAKE_SOURCE_DIR}/include/zooCmd_osg/OsgEarthSupport.h
	${PROJECT_INCLUDE}/InputDevice.h
	${PROJECT_INCLUDE}/BuiltinCmd.h
	${PROJECT_INCLUDE}/OsgComponentImpls.h
	${PROJECT_INCLUDE}/OsgEarthUtilsImpl.h
	${PROJECT_INCLUDE}/OsgEarthContextImpl.h
	${PROJECT_INCLUDE}/StarrySky.h
	${PROJECT_INCLUDE}/NebulaNode.h
	${PROJECT_INCLUDE}/CameraManipulatorManager.h
)

set(ZOOCMD_OSG_CPP
	${PROJECT_SRC}/InputDevice.cpp
	${PROJECT_SRC}/BuiltinCmd.cpp
	${PROJECT_SRC}/OsgComponentImpls.cpp
	${PROJECT_SRC}/OsgEarthUtilsImpl.cpp
	${PROJECT_SRC}/OsgEarthContextImpl.cpp
	${PROJECT_SRC}/StarrySky.cpp
	${PROJECT_SRC}/NebulaNode.cpp
	${PROJECT_SRC}/CameraManipulatorManager.cpp
)

add_library(
    ${OUT_NAME} SHARED 
    ${ZOOCMD_OSG_H}
    ${ZOOCMD_OSG_CPP}
)
ADD_DEFINITIONS(-DNEED_OSGEARTH_LIBRARY)

target_link_libraries(
	${OUT_NAME}
	zoo
	zooCmd
)

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
