OBJ_DIR = ./_obj/
OUT_DIR = ./_bin/
EXE_NAME = a.exe

CC = g++

MAIN_SRC = ./test/Window_Test.cpp
EXT_SRC = ./ext/cpp/serial_json.cpp ./ext/cpp/RenderingSDL.cpp

VENDOR_INC = asio json sdl box2d
VENDOR_LIB = sdl/lib box2d/lib

VENDOR_BIN = sdl2dll box2d
SYSTEM_BIN = gdi32 ws2_32 wsock32 user32 kernel32 shell32

_vinc = $(foreach name, $(VENDOR_INC),-I./vendor/$(name)/include)
_vlib = $(foreach name, $(VENDOR_LIB),-L./vendor/$(name))
_vbin = $(foreach name, $(VENDOR_BIN),-l$(name))
_sbin = $(foreach name, $(SYSTEM_BIN),-l$(name))

main:
# if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
	if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)"
	$(CC) $(_vinc) $(MAIN_SRC) $(EXT_SRC) $(_vlib) $(_vbin) $(_sbin) -o$(OUT_DIR)$(EXE_NAME) -g
# copy the vendor bins to OUT DIR

clean:
# if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	if exist "$(OUT_DIR)" rmdir /s /q "$(OUT_DIR)"