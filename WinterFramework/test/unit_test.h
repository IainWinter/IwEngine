#pragma once

#include <stdio.h>

struct unit_test_state
{
	int test_count = 0;
	int failed_count = 0;
	int passed_count = 0;
	bool failed = false;

	void reset()
	{
		failed = false;
	}

	static unit_test_state& instance()
	{
		static unit_test_state inst;
		return inst;
	}
};

#define _ut_state unit_test_state::instance()

inline void print_fail()   { printf("\033[0;31m failed \033[0m\n"); }
inline void print_passed() { printf("\033[0;32m passed \033[0m\n"); }

#define EPSILON .0005

#define run_test(test) \
	_ut_state.test_count++;\
	printf("%d. Running test '%s' ...", _ut_state.test_count, #test);\
	_ut_state.reset();\
	test();\
	if (_ut_state.failed) {\
		_ut_state.failed_count++;\
	}\
	else {\
		print_passed();\
		_ut_state.passed_count++;\
	}\

#define assert_equal(a, b) \
	_ut_state.failed = !(a == b);\
	if (_ut_state.failed) {\
		print_fail();\
		printf("\tValue mismatch '%s' = %d but '%s' = %d.\n", #a, a, #b, b);\
		_ut_state.failed = true;\
		return;\
	}

#define assert_near_e(a, b, e) \
	_ut_state.failed = fabsf(a - b) > e;\
	if (_ut_state.failed) {\
		print_fail();\
		printf("Value too far '%s' = %f but '%s' = %f. Difference of %f with an epsilon of %f.\n", #a, a, #b, b, fabsf(a - b), e);\
		_ut_state.failed = true;\
		return;\
	}

#define assert_near(a, b) assert_near_e(a, b, EPSILON)