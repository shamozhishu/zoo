###########################################################################
#                                                                         #
#                              zooCmdShell                                #
#                                                                         #
###########################################################################
set(OUT_NAME zooCmdShell)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB ZOOCMDSHELL_CPP "*.cpp")
           
add_executable(
	${OUT_NAME}
	${ZOOCMDSHELL_CPP}
)

target_link_libraries(
	${OUT_NAME}
	zooCmdLoader
)

INSTALL(TARGETS ${OUT_NAME} RUNTIME DESTINATION bin/$<CONFIGURATION>)
