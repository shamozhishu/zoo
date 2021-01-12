###########################################################################
#                                                                         #
#                               zooCmd_osg                                #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmd_osg)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs)

include_directories("${3RD_PARTY_DIR}/include")
include_directories("${OSG_DIR}/include")
include_directories("${OSGEARTH_DIR}/include")

link_directories("${3RD_PARTY_DIR}/lib")
link_directories("${OSG_DIR}/lib")
link_directories("${OSGEARTH_DIR}/lib")

include(Find3rdPartyDependencies)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/lex_MaterialScript.cpp")
	file(WRITE "${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/lex_MaterialScript.cpp")
endif()
if(NOT EXISTS "${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/yacc_MaterialScript.cpp")
	file(WRITE "${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/yacc_MaterialScript.cpp")
endif()
if(NOT EXISTS "${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/yacc_MaterialScript.hpp")
	file(WRITE "${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/yacc_MaterialScript.hpp")
endif()

file(GLOB ZOOCMD_OSG_H "*.h" "${CMAKE_SOURCE_DIR}/include/zooCmd_osg/*.h" "${CMAKE_SOURCE_DIR}/include/UniversalGlobalServices.h")
file(GLOB ZOOCMD_OSG_CPP "*.cpp")

add_library(
    ${OUT_NAME} SHARED 
    ${ZOOCMD_OSG_H}
    ${ZOOCMD_OSG_CPP}
	"${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/lex_MaterialScript.cpp"
	"${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/yacc_MaterialScript.cpp"
	"${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/yacc_MaterialScript.hpp"
)
ADD_DEFINITIONS(-DNEED_OSGEARTH_LIBRARY)

target_link_libraries(
	${OUT_NAME}
	zoo
	zooCmd
	osgQt
	OpenGL32.Lib
)

if (WIN32)
add_custom_command(TARGET ${OUT_NAME}
	PRE_BUILD
	COMMAND win_bison --output=${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/yacc_MaterialScript.cpp --defines --yacc -dv ${CMAKE_CURRENT_SOURCE_DIR}/MaterialScript.y
	COMMAND win_flex --outfile=${CMAKE_BINARY_DIR}/plugins/zooCmd_osg/lex_MaterialScript.cpp --nounistd --wincompat ${CMAKE_CURRENT_SOURCE_DIR}/MaterialScript.l
)
endif()

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
