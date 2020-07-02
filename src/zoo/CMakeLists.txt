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

set(ZOO_H 
	${PROJECT_INCLUDE}/Any.h
	${PROJECT_INCLUDE}/BitState.h
	${PROJECT_INCLUDE}/Block.h
	${PROJECT_INCLUDE}/Common.h
	${PROJECT_INCLUDE}/Component.h
	${PROJECT_INCLUDE}/DatabaseCSV.h
    ${PROJECT_INCLUDE}/DynLib.h
	${PROJECT_INCLUDE}/DynLibManager.h
	${PROJECT_INCLUDE}/Interlock.h
	${PROJECT_INCLUDE}/Log.h
	${PROJECT_INCLUDE}/Reflection.h
	${PROJECT_INCLUDE}/Sigslot.h
	${PROJECT_INCLUDE}/Singleton.h
	${PROJECT_INCLUDE}/Support.h
	${PROJECT_INCLUDE}/Timer.h
	${PROJECT_INCLUDE}/UserData.h
    ${PROJECT_INCLUDE}/Utils.h
	${CMAKE_SOURCE_DIR}/src/cJSON/cJSON.h
	${CMAKE_SOURCE_DIR}/src/cJSON/CJsonObject.h
)

set(ZOO_CPP
	${PROJECT_SRC}/Component.cpp
	${PROJECT_SRC}/DatabaseCSV.cpp
	${PROJECT_SRC}/DynLib.cpp
	${PROJECT_SRC}/DynLibManager.cpp
	${PROJECT_SRC}/Log.cpp
	${PROJECT_SRC}/Reflection.cpp
	${PROJECT_SRC}/Sigslot.cpp
	${PROJECT_SRC}/UserData.cpp
	${PROJECT_SRC}/Utils.cpp
	${CMAKE_SOURCE_DIR}/src/cJSON/cJSON.c
	${CMAKE_SOURCE_DIR}/src/cJSON/CJsonObject.cpp
)
           
if(${BUILD_SHARED_LIBS} STREQUAL "On")
    add_library(
    	${OUT_NAME} SHARED 
    	${ZOO_H}
    	${ZOO_CPP}
		)
    ADD_DEFINITIONS(-DZOO_NONCLIENT_BUILD)
else()
    add_library(
    	${OUT_NAME} STATIC 
    	${ZOO_H}
    	${ZOO_CPP}
    	)
    ADD_DEFINITIONS(-DZOO_NONCLIENT_BUILD_STATIC)
endif()

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
INSTALL(FILES ${ZOO_H} DESTINATION include/zoo)
