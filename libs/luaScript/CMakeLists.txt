###########################################################################
#                                                                         #
#                               luaScript                                 #
#                                                                         #
###########################################################################
if (OPTION_LUASCRIPT)

set(OUT_NAME luaScript)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/libs)

include(Find3rdPartyDependencies)
FIND_DEPENDENCY(LUA lua.h "lua;tolua++" ${3RD_PARTY_DIR} "d" "")

SET(TOLUA_INCLUDE_DIR "" CACHE PATH "Set to tolua++ include install path")
IF(NOT TOLUA_INCLUDE_DIR)
	message(FATAL_ERROR "TOLUA_INCLUDE_DIR need to set!!!")
ENDIF()

SET(TOLUA_LIBRARY "" CACHE FILEPATH "Set to tolua++ lib install file")
IF(NOT TOLUA_LIBRARY)
	message(FATAL_ERROR "TOLUA_LIBRARY need to set!!!")
ENDIF()

SET(TOLUA_LIBRARY_DEBUG "" CACHE FILEPATH "Set to tolua++ lib install file")
IF(NOT TOLUA_LIBRARY_DEBUG)
	message(FATAL_ERROR "TOLUA_LIBRARY_DEBUG need to set!!!")
ENDIF()

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

file(GLOB LUASCRIPT_H "*.h")
file(GLOB LUASCRIPT_CPP "*.cpp")
	  
add_library(
    	${OUT_NAME} STATIC 
    	${LUASCRIPT_H}
    	${LUASCRIPT_CPP}
)

target_link_libraries(
	${OUT_NAME}
	zoo
	debug ${LUA_LIBRARY_DEBUG} optimized ${LUA_LIBRARY}
	debug ${TOLUA_LIBRARY_DEBUG} optimized ${TOLUA_LIBRARY}
)

INSTALL(TARGETS ${OUT_NAME} ARCHIVE DESTINATION lib)
INSTALL(FILES ${LUASCRIPT_H} DESTINATION include/luaScript)

endif()
