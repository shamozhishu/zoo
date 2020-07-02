###########################################################################
#                                                                         #
#                              zooCmdShell                                #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmdShell)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

SET(PROJECT_SRC ${CMAKE_SOURCE_DIR}/applications/zooCmdShell)

set(ZOO_CPP
	${PROJECT_SRC}/main.cpp
)
           
add_executable(
	${OUT_NAME}
	${ZOO_CPP}
)

target_link_libraries(
	${OUT_NAME}
	zooCmdLoader
)

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
