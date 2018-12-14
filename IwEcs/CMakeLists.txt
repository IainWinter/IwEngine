cmake_minimum_required(VERSION 3.8)

project(IwEcs)

#Set source variables
set(SRC
	"src/ecs_manager.cpp")
set(HEADERS
	"include/chunk.h"
	"include/component_array.h"
	"include/component_registry.h"
	"include/component_view.h"
	"include/ecs_manager.h"
	"include/entity_manager.h"
	"include/iwecs.h")
set(POST_INSTALL_INCLUDE "${PROJECT_NAME}")

#Add libraries
add_library(${PROJECT_NAME} SHARED ${SRC})
add_library(Iw::Ecs ALIAS ${PROJECT_NAME})

#Set include directorys
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories("X:/include")

#Set target include directory for client
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

#Set target compile time constants
if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC  IW_PLATFORM_WINDOWS=1)
	target_compile_definitions(${PROJECT_NAME} PRIVATE IW_BUILD_DLL=1)
elseif(APPLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC  IW_PLATFORM_APPLE=1)
elseif(UNIX AND NOT APPLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC  IW_PLATFORM_UNIX=1)
endif()

#Set target compile features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

#Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

#Install locations
install(TARGETS ${PROJECT_NAME}
	RUNTIME       DESTINATION bin/${POST_INSTALL}
	LIBRARY       DESTINATION bin/${POST_INSTALL}
	ARCHIVE       DESTINATION lib/${POST_INSTALL}
	FRAMEWORK     DESTINATION lib/${POST_INSTALL}
	BUNDLE        DESTINATION bin/${POST_INSTALL}
	PUBLIC_HEADER DESTINATION include/${POST_INSTALL_INCLUDE})