#pragma

#define S1(x) #x
#define S2(x) S1(x)
#define __LOCATION __FILE__ " : " S2(__LINE__)
