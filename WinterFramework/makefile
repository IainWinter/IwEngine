OBJ_DIR = ./_obj/
OUT_DIR = ./_bin/
EXE_NAME = a.exe

CC = g++ -std=c++17

MAIN_SRC = ./test/Window_Test.cpp

EXT_SRC = serial_json.cpp Time.cpp

# I dont really like how this is the only lib that directly links its file...
# wish it could just be a static a. like the others

VENDOR_SRC = ./vendor/glad/src/glad.c ./vendor/imgui/backends/imgui_impl_sdl.cpp ./vendor/imgui//backends/imgui_impl_opengl3.cpp

VENDOR_INC = sdl box2d imgui hitbox entt stb glm glad asio json
VENDOR_LIB = sdl box2d imgui hitbox 

VENDOR_BIN = sdl2dll box2d imgui hitbox
SYSTEM_BIN = gdi32 ws2_32 wsock32 user32 kernel32 shell32

DEFINES = IW_DEBUG IW_ENTITY_DEBUG

_esrc = $(foreach name, $(EXT_SRC),./ext/cpp/$(name))

_vinc = $(foreach name, $(VENDOR_INC),-I./vendor/$(name)/include)
_vlib = $(foreach name, $(VENDOR_LIB),-L./vendor/$(name)/lib)
_vbin = $(foreach name, $(VENDOR_BIN),-l$(name))
_sbin = $(foreach name, $(SYSTEM_BIN),-l$(name))
_defs = $(foreach name, $(DEFINES),-D$(name))

main:
# if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
	if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)"
	$(CC) $(_vinc) $(MAIN_SRC) $(_esrc) $(VENDOR_SRC) $(_vlib) $(_vbin) $(_sbin) $(_defs) -o$(OUT_DIR)$(EXE_NAME) -g
# copy the vendor bins to OUT DIR

clean:
# if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	if exist "$(OUT_DIR)" rmdir /s /q "$(OUT_DIR)"