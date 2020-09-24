###########################################################################
#                                                                         #
#                               zooCmd_osg                                #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmd_osg)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

include_directories("${3RD_PARTY_DIR}/include")
include_directories("${OSG_DIR}/include")
include_directories("${OSGEARTH_DIR}/include")

link_directories("${3RD_PARTY_DIR}/lib")
link_directories("${OSG_DIR}/lib")
link_directories("${OSGEARTH_DIR}/lib")

include(Find3rdPartyDependencies)

file(GLOB ZOOCMD_OSG_H "*.h" "${CMAKE_SOURCE_DIR}/include/zooCmd_osg/*.h" "${CMAKE_SOURCE_DIR}/include/UniversalGlobalServices.h")
file(GLOB ZOOCMD_OSG_CPP "*.cpp" "*.l" "*.y")

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
	OpenGL32.Lib
)

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
