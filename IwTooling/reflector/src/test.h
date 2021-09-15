#pragma once

//#include "clang-c/Index.h"

enum test_enum {
	D1,
	D2,
	D3
};

template<typename _t, size_t _s, int _i, test_enum _d>
struct test_base {
	float base_x;
	int base_y;
	_t base_t;
};

typedef test_base<int, 2, 1, D2> test_base_int;
using test_base_float = test_base<float, 5, 2, D1>;

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
