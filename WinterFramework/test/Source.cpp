#include "unit_test.h"

#include "../util/free_list.h"

void test_free_list_mark()
{
	free_list<int> freelist(0, 30);

	// range should resize if marked adjacent
	// mark (0, 10)
	// mark (20, 10)
	// (0 30] -> (10 30] -> (10 20]

	freelist.mark(0, 10);
	freelist.mark(20, 10);

	assert_equal(freelist.m_list.size(), 1);
	assert_equal(freelist.m_list.at(0).m_begin, 10);
	assert_equal(freelist.m_list.at(0).m_size,  10);

	freelist.reset();
	freelist.mark(0, 10);

	// range should split if marked in the middle
	// mark (15, 10)
	// (10 30] -> (10 15] (25 30]

	freelist.mark(15, 10);

	assert_equal(freelist.m_list.size(), 2);
	assert_equal(freelist.m_list.at(0).m_begin, 10);
	assert_equal(freelist.m_list.at(0).m_size, 5);
	assert_equal(freelist.m_list.at(1).m_begin, 25);
	assert_equal(freelist.m_list.at(1).m_size, 5);

	// ramge should be removed if marked fully
	// mark (25, 5)
	// (10 15] (25 30] -> (10 15]

	freelist.mark(25, 5);

	assert_equal(freelist.m_list.size(), 1);
	assert_equal(freelist.m_list.at(0).m_begin, 10);
	assert_equal(freelist.m_list.at(0).m_size, 5);
}

void test_free_list_unmark()
{
	free_list<int> freelist(0, 30);
	freelist.mark(20, 10);

	// range should be created if not adjacent
	// unmark (25 5)
	// (0 20] -> (0 20] (25 30]
	
	freelist.unmark(25, 5);
	
	assert_equal(freelist.m_list.size(), 2);
	assert_equal(freelist.m_list.at(0).m_begin, 0);
	assert_equal(freelist.m_list.at(0).m_size, 20);
	assert_equal(freelist.m_list.at(1).m_begin, 25);
	assert_equal(freelist.m_list.at(1).m_size, 5);

	// range should expand if adjacent left & right
	// unmark (20 2)
	// unmark (23 2)
	// (0 20] (25 30] -> (0 22] (23 30]

	freelist.unmark(20, 2);
	freelist.unmark(23, 2);
	
	assert_equal(freelist.m_list.size(), 2);
	assert_equal(freelist.m_list.at(0).m_begin, 0);
	assert_equal(freelist.m_list.at(0).m_size, 22);
	assert_equal(freelist.m_list.at(1).m_begin, 23);
	assert_equal(freelist.m_list.at(1).m_size, 7);

	// range should combine left and right is full adjacent
	// unmark (22 1)
	// (0 22] (23 30] -> (0 30]

	freelist.unmark(22, 1);

	assert_equal(freelist.m_list.size(), 1);
	assert_equal(freelist.m_list.at(0).m_begin, 0);
	assert_equal(freelist.m_list.at(0).m_size, 30);
}

void test_free_list_helpers()
{
	free_list<int> freelist(0, 30);
	
	assert_equal(freelist.mark_first(10), 0);

	assert_equal(freelist.m_list.size(), 1);
	assert_equal(freelist.m_list.at(0).m_begin, 10);
	assert_equal(freelist.m_list.at(0).m_size, 20);

	assert_equal(freelist.first_fits(1), 10);

	assert_equal(freelist.has_space(100), false);
	assert_equal(freelist.has_space(5), true);

	assert_equal(freelist.is_marked(20), false);
	assert_equal(freelist.is_marked(5), true);
}

// #include "../util/linear_allocator.h"

// void test_linear_alloc()
// {
// 	linear_allocator allocator(1024);

// 	allocator.
// }

int main()
{
	run_test(test_free_list_mark);
	run_test(test_free_list_unmark);
	run_test(test_free_list_helpers);



	return 0;
}
