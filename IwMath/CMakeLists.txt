cmake_minimum_required(VERSION 3.8)

project(IwMath)

#Set source variables
set(SRC
	"src/iwmath.cpp"
	"src/matrix2.cpp"
	"src/matrix3.cpp"
	"src/matrix4.cpp"
	"src/quaternion.cpp"
	"src/vector2.cpp"
	"src/vector3.cpp"
	"src/vector4.cpp")
set(HEADERS
	"include/iwmath.h"
	"include/matrix2.h"
	"include/matrix3.h"
	"include/matrix4.h"
	"include/quaternion.h"
	"include/vector2.h"
	"include/vector3.h"
	"include/vector4.h")
set(POST_INSTALL_INCLUDE "${PROJECT_NAME}")

#Add libraries
add_library(${PROJECT_NAME} SHARED ${SRC})
add_library(Iw::Math ALIAS ${PROJECT_NAME})

#Set include directorys
include_directories(${PROJECT_SOURCE_DIR}/include)

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