###########################################################################
#                                                                         #
#                               toluaInput                                #
#                                                                         #
###########################################################################
if (OPTION_TOLUAINPUT)

set(OUT_NAME toluaInput)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs)
include_directories(${TOLUA_INCLUDE_DIR})

if(WIN32 AND NOT ANDROID)
	if(DX_SDK_INCLUDE)
		include_directories(${DX_SDK_INCLUDE})
	else()
		message(FATAL_ERROR "DX_SDK_INCLUDE need to set!!!")
	endif()
	
	if( NOT DX_SDK_LIB)
		message(FATAL_ERROR "DX_SDK_LIB need to set!!!")
	endif()
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/libs/toluaInput/ToLuaInput.cpp")
	file(WRITE "${CMAKE_BINARY_DIR}/libs/toluaInput/ToLuaInput.cpp")
endif()

file(GLOB INPUT_H "*.h")
file(GLOB INPUT_CPP "*.cpp")
	  
add_library(
	${OUT_NAME} STATIC 
	${INPUT_H}
	${INPUT_CPP}
	ToLuaInput.cpp
	ToLuaInput.pkg
)

target_link_libraries(
	${OUT_NAME}
	${DX_SDK_LIB}/dinput8.lib
	${DX_SDK_LIB}/dxguid.lib
)

if (WIN32)
add_custom_command(TARGET ${OUT_NAME}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_CURRENT_SOURCE_DIR}/Input.h" "${CMAKE_BINARY_DIR}/libs/toluaInput"
	COMMAND tolua++ -n ToLuaInput
	-o "${CMAKE_BINARY_DIR}/libs/toluaInput/ToLuaInput.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/ToLuaInput.pkg"
)
endif()

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
INSTALL(FILES ${INPUT_H} DESTINATION include/toluaInput)

endif()
