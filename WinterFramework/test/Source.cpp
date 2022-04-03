#include "../Entity.h"
#include <cstdio>

void print_list(const free_list<int>& list)
{
	printf("\n");
	for (const auto& range : list.m_list)
	{
		printf("(%d, %d] ", range.m_begin, range.m_begin + range.m_size);
	}
}

int main()
{
	free_list<int> test;

	for (int i = 0; i < 29; i++)
	{
		test.mark(i, 1);
	}

	// test.mark(5, 5);
	// print_list(test);
	// test.mark(11, 1);
	// test.mark(12, 3);
	// print_list(test);
	// test.unmark(5, 5);
	// print_list(test);
	// test.mark(0, 2);

	// //test.mark(29, 1);
	// print_list(test);

	test.unmark(7, 3);
	print_list(test);
	test.unmark(0, 2);
	print_list(test);
	test.unmark(10, 9);
	print_list(test);

	for (int i = 18; i >= 7; i--)
	{
		test.mark(i, 1);
		print_list(test);
	}

	test.mark(29, 1);
	print_list(test);
	test.mark(0, 2);
	print_list(test);

	test.reset();
	print_list(test);

	pool_allocator pool(1);
	pool.m_block_size = 10;

	int* ints = pool.alloc<int>(100);
	char* block = pool.alloc_block();
	char* bytes = pool.alloc_bytes(100);

	linear_allocator lin(100);
	lin.m_block_size = 10;
	lin.alloc_block();
	lin.alloc_block();
	char* gggg = lin.alloc_block();
	lin.alloc_block();
	lin.alloc_block();

	lin.free_block(gggg);

	for (linear_allocator_iterator itr(lin); itr.more(); itr.next())
	{
		printf("\n%p", itr.get_bytes());
	}

	for (pool_allocator_iterator itr(pool); itr.more(); itr.next())
	{
		printf("\n%p", itr.get_bytes());
	}

	entity_manager manager;

	manager.create<int>();
	manager.create<int>();
	manager.create<int, float>();
	entity ef = manager.create<int, float>();
	entity e = manager.create<int>();

	e.get<int>() += 1;
	ef.get<int>() = 10;

	e.destroy();

	for (auto [e, i] : manager.query<int>().with_entity())
	{
		printf("\n%d", i);
	}

	return 0;
}
