cmake_minimum_required (VERSION 3.8)

project(Sandbox)

#Source cpps
set(SRC
	"src/Source.cpp")

#Add executable
add_executable (${PROJECT_NAME} ${SRC})

#Include libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

#Link libraries
target_link_libraries(${PROJECT_NAME} Iw::Engine)
target_link_libraries(${PROJECT_NAME} Iw::Ecs)

#Set compile features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

#Copy libraries next to executable
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:Iw::Engine>
		$<TARGET_FILE_DIR:${PROJECT_NAME}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:Iw::Ecs>
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)