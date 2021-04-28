###########################################################################
#                                                                         #
#                             zooCmd_osg-earth                            #
#                                                                         #
###########################################################################
if (OPTION_ZOOCMD_OSG_EARTH)

set(OUT_NAME zooCmd_osg-earth)

include_directories("${3RD_PARTY_DIR}/include")
include_directories("${OSG_DIR}/include")
include_directories("${OSGEARTH_DIR}/include")

link_directories("${3RD_PARTY_DIR}/lib")
link_directories("${OSG_DIR}/lib")
link_directories("${OSGEARTH_DIR}/lib")

include(Find3rdPartyDependencies)

file(GLOB ZOOCMD_PLUGIN_LIB_H "*.h")
file(GLOB ZOOCMD_PLUGIN_LIB_CPP "*.cpp")
    
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
)

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)

endif()
