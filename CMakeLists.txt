cmake_minimum_required(VERSION 3.8)

project(IwEngine)

#Options for every project
option(IW_BUILD_SANDBOX "Build Sandbox.exe" ON)

if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	set(ARCHITECTURE "x64")
else()
	set(ARCHITECTURE "x86")
endif()

set(POST_INSTALL "${ARCHITECTURE}")

#Add libraries to build
add_subdirectory(IwLog)
add_subdirectory(IwUtil)
add_subdirectory(IwEvents)
add_subdirectory(IwMath)
add_subdirectory(IwGraphics)
add_subdirectory(IwEcs)
add_subdirectory(IwInput)
add_subdirectory(IwEngine)

#Only build the sandbox for testing
if(IW_BUILD_SANDBOX)
	add_subdirectory(Sandbox)
endif()
