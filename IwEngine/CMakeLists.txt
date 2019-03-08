cmake_minimum_required(VERSION 3.8)

project(IwEngine 
	VERSION 1.0.0
	LANGUAGES CXX)

add_library(${PROJECT_NAME} SHARED
	"src/Platform/WindowsWindow.cpp"
	"src/Application.cpp"
	"src/Layer.cpp"
	"src/LayerStack.cpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src)

if(${CMAKE_BUILD_TYPE} EQUAL "DEBUG")
	target_compile_definitions(${PROJECT_NAME}
		PRIVATE IW_DEBUG)
endif()

if(WIN32)
	target_compile_definitions(${PROJECT_NAME}
		PUBLIC  IW_PLATFORM_WINDOWS
		PRIVATE IW_BUILD_DLL)
elseif(APPLE)
	target_compile_definitions(${PROJECT_NAME}
		PUBLIC IW_PLATFORM_APPLE)
elseif(UNIX AND NOT APPLE)
	target_compile_definitions(${PROJECT_NAME}
		PUBLIC IW_PLATFORM_UNIX)
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

find_package(OpenGL     REQUIRED)
find_package(GLEW       REQUIRED)
find_package(imgui      REQUIRED)
#find_package(IwLog      REQUIRED)
#find_package(IwEvents   REQUIRED)
#find_package(IwGraphics REQUIRED)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE
		${OPENGL_gl_LIBRARY}
		${GLEW_LIBRARY}
		#imgui
		#IwLog
		#IwEvents
		#IwGraphics)
		)
