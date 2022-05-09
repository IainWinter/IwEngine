mkdir lib
cd lib
g++ -I../include -I../src ../src/common/*.cpp ../src/collision/*.cpp ../src/dynamics/*.cpp ../src/rope/*.cpp -c
ar rvs libbox2d.a ./*.o
del *.o