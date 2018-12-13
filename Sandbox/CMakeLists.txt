cmake_minimum_required (VERSION 3.8)

project(Sandbox)

set(SRC "src/Source.cpp")

add_executable (${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

target_link_libraries(${PROJECT_NAME} IwEngine::Engine)