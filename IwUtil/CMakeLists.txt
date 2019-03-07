cmake_minimum_required(VERSION 3.8)

project(IwUtil)

#Set source variables
set(INCLUDE_DIRS
	"${PROJECT_SOURCE_DIR}/include/")
set(HEADERS
	"include/iwutil.h"
	"include/action.h"
	"include/set/sparse_set.h"
	"include/type/type_group.h"
	"include/type/type_index.h"
	"include/type/type_traits.h")
set(POST_INSTALL_INCLUDE
	"${PROJECT_NAME}")

#Add libraries
add_library(${PROJECT_NAME} INTERFACE)

#Set target include directory for client
target_include_directories(${PROJECT_NAME} INTERFACE ${INCLUDE_DIRS})

#Set target compile time constants
if(WIN32)
	target_compile_definitions(${PROJECT_NAME} INTERFACE IW_PLATFORM_WINDOWS = 1)
	target_compile_definitions(${PROJECT_NAME} INTERFACE IW_BUILD_DLL = 1)
elseif(APPLE)
	target_compile_definitions(${PROJECT_NAME} INTERFACE IW_PLATFORM_APPLE = 1)
elseif(UNIX AND NOT APPLE)
	target_compile_definitions(${PROJECT_NAME} INTERFACE IW_PLATFORM_UNIX = 1)
endif()

#Set target compile features
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

#Install locations
install(FILES ${HEADERS} DESTINATION include/${POST_INSTALL_INCLUDE})
