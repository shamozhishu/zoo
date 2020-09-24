###########################################################################
#                                                                         #
#                                zooCmd                                   #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmd)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB ZOOCMD_H "*.h")
file(GLOB ZOOCMD_CPP "*.cpp")
	  
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
