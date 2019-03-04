cmake_minimum_required(VERSION 3.8)

project(IwEngine)

#Set source variables
set(INCLUDE_DIRS
	"${PROJECT_SOURCE_DIR}/include/")
set(SRC 
	"src/Application.cpp"
	"src/Platform/WindowsWindow.cpp")
set(HEADERS
	"include/Application.h"
	"include/Core.h"
	"include/EntryPoint.h"
	"include/Window.h"
	"include/Platform/Windows/WindowsWindow.h")
set(POST_INSTALL_INCLUDE
	"${PROJECT_NAME}")

find_package(OpenGL)
find_package(GLEW)

#Add libraries
add_library(${PROJECT_NAME} SHARED ${SRC})
add_library(Iw::Engine ALIAS ${PROJECT_NAME})

#Set include directorys
include_directories(${INCLUDE_DIRS})

#Set target include directory for client
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

#Set target link libraries
target_link_libraries(
    ${PROJECT_NAME}
    ${OPENGL_gl_LIBRARY}
    ${GLEW_LIBRARY}
	"Iw::Log")

#Set target compile time constants
if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC  IW_PLATFORM_WINDOWS = 1)
	target_compile_definitions(${PROJECT_NAME} PRIVATE IW_BUILD_DLL = 1)
elseif(APPLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC  IW_PLATFORM_APPLE = 1)
elseif(UNIX AND NOT APPLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC  IW_PLATFORM_UNIX = 1)
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
