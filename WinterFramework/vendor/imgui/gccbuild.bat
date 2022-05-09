mkdir lib
cd lib
g++ -I../../sdl/include/sdl -I../include/imgui -I../src ../src/*.cpp -c
ar rvs libimgui.a ./*.o
del *.o

del ../include/*impl*.h
copy ..\backends\imgui_impl_sdl.h ..\include\imgui
copy ..\backends\imgui_impl_opengl3.h ..\include\imgui