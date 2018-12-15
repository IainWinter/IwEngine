cmake_minimum_required(VERSION 3.8)

project(IwInput)

#Set source variables
set(INCLUDE_DIRS
	"${PROJECT_SOURCE_DIR}/include/")
set(SRC 
	"src/input_manager.cpp"
	"src/impl/impl_input_context.cpp"
	"src/impl/impl_input_device.cpp"
	"src/impl/impl_input_manager.cpp"
	"src/impl/impl_input_state.cpp"
	"src/impl/keyboard/impl_keyboard_win.cpp"
	"src/impl/mouse/impl_mouse_win.cpp"
	"src/impl/mouse/impl_mouse_winraw.cpp")
set(HEADERS
	"include/input_context.h"
	"include/input_device.h"
	"include/input_manager.h"
	"include/input_state.h"
	"include/iwinput.h"
	"include/keyboard.h"
	"include/mouse.h")
set(POST_INSTALL_INCLUDE
	"${PROJECT_NAME}")

#Add libraries
add_library(${PROJECT_NAME} SHARED ${SRC})
add_library(Iw::Input ALIAS ${PROJECT_NAME})

#Set include directorys
include_directories(${INCLUDE_DIRS})

#Set target include directory for client
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

#Link libraries
target_link_libraries(${PROJECT_NAME} Iw::Events)

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