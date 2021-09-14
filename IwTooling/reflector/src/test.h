#pragma once

//#include "clang-c/Index.h"

template<typename _t, size_t _s>
struct test_base {
	float base_x;
	int base_y;
	_t base_t;
};

typedef test_base<int, 2> test_base_int;

using test_base_float = test_base<float, 5>;

struct test_int : test_base_int {
	float x;
	float y;
	int xy;
};

struct test_float : test_base_float {
	int x;
	int y;
	float xy;
};
