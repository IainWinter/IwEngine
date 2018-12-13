cmake_minimum_required(VERSION 3.8)

project(IwEngine)

#Source cpps
set(SRC "src/Application.cpp")

#Add libraries
add_library(${PROJECT_NAME} SHARED ${SRC})
add_library(IwEngine::Engine ALIAS ${PROJECT_NAME})


#Set include directorys
include_directories(${PROJECT_SOURCE_DIR}/include)

#Set include directory for client
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

#Set compile time constants
if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC  IW_PLATFORM_WINDOWS=1)
	target_compile_definitions(${PROJECT_NAME} PRIVATE IW_BUILD_DLL=1)
elseif(APPLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC  IW_PLATFORM_APPLE=1)
elseif(UNIX AND NOT APPLE)
	target_compile_definitions(${PROJECT_NAME} PUBLIC  IW_PLATFORM_UNIX=1)
endif()

#Set compile features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)