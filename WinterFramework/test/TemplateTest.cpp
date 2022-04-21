#include <stdio.h>

template<typename _t>
struct test_thing {};

void test_3(int)  { printf("A"); }
template<typename _t> void test_2(_t) { printf("A"); }

template<typename _t>
void test()
{
	//test_2<_t>({}); // prints: ABC
	::test_2<_t>({}); // prints: ABA    <-- namespace op, searching different?
}

template<>            void test_2(double)         { printf("B"); }
template<typename _t> void test_2(test_thing<_t>) { printf("C"); }

void test_3(double) { printf("B"); }

int main()
{
    test<int>();
    test<double>();
    test<test_thing<int>>();

    return 0;
}