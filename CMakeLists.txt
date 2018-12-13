cmake_minimum_required(VERSION 3.8)

project(IwEngine)

option(IW_BUILD_SANDBOX "Build Sandbox.exe" ON)

add_subdirectory(IwEngine)

if(IW_BUILD_SANDBOX)
	add_subdirectory(Sandbox)
endif()