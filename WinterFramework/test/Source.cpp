#include "../Entity.h"
#include <cstdio>

#include "unit_test.h"

void test_free_list_mark()
{
	free_list<int> freelist(0, 30);

	// range should resize if marked adjacent
	// mark (0, 10)
	// mark (20, 10)
	// (0 30] -> (10 20]

	freelist.mark(0, 10);
	freelist.mark(20, 10);

	assert_equal(freelist.m_list.size(), 1);
	assert_equal(freelist.m_list.at(0).m_begin, 10);
	assert_equal(freelist.m_list.at(0).m_size,  10);

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
	assert_equal(freelist.m_list.at(1).m_size, 30);

	// range should expand if adjacent left & right
	// (0 20] (25 30]


}

int main()
{
	run_test(test_free_list_mark);
	run_test(test_free_list_unmark);

	// free_list<int> test;

	// for (int i = 0; i < 29; i++)
	// {
	// 	test.mark(i, 1);
	// }

	// // test.mark(5, 5);
	// // print_list(test);
	// // test.mark(11, 1);
	// // test.mark(12, 3);
	// // print_list(test);
	// // test.unmark(5, 5);
	// // print_list(test);
	// // test.mark(0, 2);

	// // //test.mark(29, 1);
	// // print_list(test);

	// test.unmark(7, 3);
	// print_list(test);
	// test.unmark(0, 2);
	// print_list(test);
	// test.unmark(10, 9);
	// print_list(test);

	// for (int i = 18; i >= 7; i--)
	// {
	// 	test.mark(i, 1);
	// 	print_list(test);
	// }

	// test.mark(29, 1);
	// print_list(test);
	// test.mark(0, 2);
	// print_list(test);

	// test.reset();
	// print_list(test);

	// pool_allocator pool(1);
	// pool.m_block_size = 10;

	// int* ints = pool.alloc<int>(100);
	// char* block = pool.alloc_block();
	// char* bytes = pool.alloc_bytes(100);

	// linear_allocator lin(100);
	// lin.m_block_size = 10;
	// lin.alloc_block();
	// lin.alloc_block();
	// char* gggg = lin.alloc_block();
	// lin.alloc_block();
	// lin.alloc_block();

	// lin.free_block(gggg);

	// for (linear_allocator_iterator itr(lin); itr.more(); itr.next())
	// {
	// 	printf("\n%p", itr.get_bytes());
	// }

	// for (pool_allocator_iterator itr(pool); itr.more(); itr.next())
	// {
	// 	printf("\n%p", itr.get_bytes());
	// }

	// entity_manager manager;

	// manager.create<int>();
	// manager.create<int>();
	// manager.create<int, float>();
	// entity ef = manager.create<int, float>();
	// entity e = manager.create<int>();

	// e.get<int>() += 1;
	// ef.get<int>() = 10;

	// e.destroy();

	// for (auto [e, i] : manager.query<int>().with_entity())
	// {
	// 	printf("\n%d", i);
	// }

	return 0;
}
