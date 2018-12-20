cmake_minimum_required(VERSION 3.8)

project(IwEcs)

#Set source variables
set(INCLUDE_DIRS
	"${PROJECT_SOURCE_DIR}/include/")
set(SRC 
	"src/entity_manager.cpp")
set(HEADERS
	"include/component_list.h"
	"include/component_registry.h"
	"include/component_view.h"
	"include/ecs_manager.h"
	"include/system.h"
	"include/system_manager.h"
	"include/entity_manager.h")
set(POST_INSTALL_INCLUDE
	"${PROJECT_NAME}")

#Add libraries
add_library(${PROJECT_NAME} SHARED ${SRC})
add_library(Iw::Ecs ALIAS ${PROJECT_NAME})

#Set include directorys
include_directories(${INCLUDE_DIRS})

#Set target include directory for client
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

#Set target compile time constants
if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC IW_PLATFORM_WINDOWS = 1)
	target_compile_definitions(${PROJECT_NAME} PRIVATE IW_BUILD_DLL = 1)
elseif(APPLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC IW_PLATFORM_APPLE = 1)
elseif(UNIX AND NOT APPLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC IW_PLATFORM_UNIX = 1)
endif()

#Link libraries
#Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC Iw::Util)

#Set target compile features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

#Install locations
install(FILES ${HEADERS} DESTINATION include/${POST_INSTALL_INCLUDE})