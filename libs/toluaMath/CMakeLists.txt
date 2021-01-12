###########################################################################
#                                                                         #
#                                toluaMath                                #
#                                                                         #
###########################################################################
set(OUT_NAME toluaMath)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs)
include_directories(${TOLUA_INCLUDE_DIR})
include_directories("${OSG_DIR}/include")

if(NOT EXISTS "${CMAKE_BINARY_DIR}/libs/toluaMath/ToLuaMath.cpp")
	file(WRITE "${CMAKE_BINARY_DIR}/libs/toluaMath/ToLuaMath.cpp")
endif()

file(GLOB MATH_PKG "*.pkg")
	  
add_library(
	${OUT_NAME} STATIC
	${MATH_PKG}
	MathWrapper.h
	ToLuaMath.cpp
)

target_link_libraries(
	${OUT_NAME}
	debug ${OSG_DIR}/lib/osgd.lib optimized ${OSG_DIR}/lib/osg.lib
)

if (WIN32)
add_custom_command(TARGET ${OUT_NAME}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_CURRENT_SOURCE_DIR}/Math.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vec2f.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vec2d.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vec3f.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vec3d.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vec4f.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vec4d.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	"${CMAKE_CURRENT_SOURCE_DIR}/Matrixf.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	"${CMAKE_CURRENT_SOURCE_DIR}/Matrixd.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	"${CMAKE_CURRENT_SOURCE_DIR}/Quat.pkg" "${CMAKE_BINARY_DIR}/libs/toluaMath"
	COMMAND tolua++ -n ToLuaMath
	-o "${CMAKE_BINARY_DIR}/libs/toluaMath/ToLuaMath.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/ToLuaMath.pkg"
)
endif()

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
