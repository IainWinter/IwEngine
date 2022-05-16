mkdir lib
cd lib
g++ -I../include -I../../glm/include ../src/*.cpp -c -g
ar rvs libhitbox.a ./*.o
del *.o