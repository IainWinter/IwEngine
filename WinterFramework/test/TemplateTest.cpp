#include <stdio.h>

template<typename _t>
struct test_thing {};

template<typename _t>
struct tag {};

template<typename _t>
void test(tag<_t>)
{
    printf("This is a test of a concrete function\n");
}

template<typename _t>
void test(tag<test_thing<_t>>);

struct concrete 
{
    virtual void test_func() = 0;
};

template<typename _t>
struct templated_struct : concrete
{
    void test_func() override
    {
        test(tag<_t>{});
    }
};

int main()
{
    concrete* t_int  = new templated_struct<int>();
    concrete* t_test = new templated_struct<test_thing<int>>();

    t_int ->test_func();
    t_test->test_func();

    return 0;
}

template<typename _t>
void test(tag<test_thing<_t>>)
{
    printf("This is a tet of a partial function\n");
}