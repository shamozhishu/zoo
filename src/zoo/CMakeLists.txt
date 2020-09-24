###########################################################################
#                                                                         #
#                                   zoo                                   #
#                                                                         #
###########################################################################
set(OUT_NAME zoo)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

SET(PROJECT_INCLUDE ${CMAKE_SOURCE_DIR}/src/zoo)
SET(PROJECT_SRC ${CMAKE_SOURCE_DIR}/src/zoo)

file(GLOB ZOO_H "*.h")
file(GLOB ZOO_CPP "*.cpp")

set(ZOO_JSON
	${CMAKE_SOURCE_DIR}/src/cJSON/cJSON.h
	${CMAKE_SOURCE_DIR}/src/cJSON/cJSON.c
	${CMAKE_SOURCE_DIR}/src/cJSON/JsonObject.h
	${CMAKE_SOURCE_DIR}/src/cJSON/JsonObject.cpp
)
           
if(${BUILD_SHARED_LIBS} STREQUAL "On")
    add_library(
    	${OUT_NAME} SHARED 
    	${ZOO_H}
    	${ZOO_CPP}
		${ZOO_JSON}
		)
    ADD_DEFINITIONS(-DZOO_NONCLIENT_BUILD)
else()
    add_library(
    	${OUT_NAME} STATIC 
    	${ZOO_H}
    	${ZOO_CPP}
		${ZOO_JSON}
    	)
    ADD_DEFINITIONS(-DZOO_NONCLIENT_BUILD_STATIC)
endif()

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
INSTALL(FILES ${ZOO_H} DESTINATION include/zoo)
