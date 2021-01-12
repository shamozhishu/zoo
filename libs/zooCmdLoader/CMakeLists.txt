###########################################################################
#                                                                         #
#                             zooCmdLoader                                #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmdLoader)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs)

file(GLOB ZOOCMDLOADER_H "*.h")
file(GLOB ZOOCMDLOADER_CPP "*.cpp")
	  
add_library(
    	${OUT_NAME} STATIC 
    	${ZOOCMDLOADER_H}
    	${ZOOCMDLOADER_CPP}
)

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
INSTALL(FILES ${ZOOCMDLOADER_H} DESTINATION include/ZooCmdLoader)
