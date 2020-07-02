###########################################################################
#                                                                         #
#                                zooCmd                                   #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmd)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

SET(PROJECT_INCLUDE ${CMAKE_SOURCE_DIR}/src/zooCmd)
SET(PROJECT_SRC ${CMAKE_SOURCE_DIR}/src/zooCmd)

set(ZOOCMD_H 
	${PROJECT_INCLUDE}/BuiltinCmd.h
	${PROJECT_INCLUDE}/Cmd.h
	${PROJECT_INCLUDE}/CmdManager.h
	${PROJECT_INCLUDE}/CmdParser.h
	${PROJECT_INCLUDE}/CmdUsage.h
	${PROJECT_INCLUDE}/InputAdapter.h
    ${PROJECT_INCLUDE}/Support.h
	${PROJECT_INCLUDE}/ZooCmd.h
)

set(ZOOCMD_CPP
	${PROJECT_SRC}/BuiltinCmd.cpp
	${PROJECT_SRC}/CmdManager.cpp
	${PROJECT_SRC}/CmdParser.cpp
	${PROJECT_SRC}/CmdUsage.cpp
	${PROJECT_SRC}/InputAdapter.cpp
	${PROJECT_SRC}/ZooCmd.cpp
)
	  
if(${BUILD_SHARED_LIBS} STREQUAL "On")
    add_library(
    	${OUT_NAME} SHARED 
    	${ZOOCMD_H}
    	${ZOOCMD_CPP}
		)
    ADD_DEFINITIONS(-DZOOCMD_NONCLIENT_BUILD)
else()
    add_library(
    	${OUT_NAME} STATIC 
    	${ZOOCMD_H}
    	${ZOOCMD_CPP}
    	)
    ADD_DEFINITIONS(-DZOOCMD_NONCLIENT_BUILD_STATIC)
endif()

target_link_libraries(
	${OUT_NAME}
	zoo
)

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
INSTALL(FILES ${ZOOCMD_H} DESTINATION include/zooCmd)
