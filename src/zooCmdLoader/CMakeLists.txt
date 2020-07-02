###########################################################################
#                                                                         #
#                             zooCmdLoader                                #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmdLoader)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

SET(PROJECT_INCLUDE ${CMAKE_SOURCE_DIR}/src/zooCmdLoader)
SET(PROJECT_SRC ${CMAKE_SOURCE_DIR}/src/zooCmdLoader)

set(ZOOCMDLOADER_H 
	${PROJECT_INCLUDE}/ZooCmdLoader.h
)

set(ZOOCMDLOADER_CPP
	${PROJECT_SRC}/ZooCmdLoader.cpp
)
	  
add_library(
    	${OUT_NAME} STATIC 
    	${ZOOCMDLOADER_H}
    	${ZOOCMDLOADER_CPP}
)

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
INSTALL(FILES ${ZOOCMDLOADER_H} DESTINATION include/ZooCmdLoader)
