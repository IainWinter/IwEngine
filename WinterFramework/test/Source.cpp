#include "../util/free_list.h"
#include "unit_test.h"

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

#include "../Asset.h"
#include "../Serial.h"
#include "../Entity.h"

struct texture
{
	int w, h;
};

void load_texture(const file_name& file, texture* texture)
{
	texture->w = 50;
	texture->h = 100;
}

void load_int(const file_name& file, int* i)
{
	*i = std::stoi(file.m_name);
}

void* load_bytes(const file_name& file, allocator* alloc)
{
	char* bytes = alloc->alloc_bytes(1000); // file length
	return bytes;
}

struct test_type
{
	float x, y;
	int z;

	std::string str;

	std::vector<int> vec_ints;
	std::vector<std::string> vec_strs;
};

// string serializer
struct serial_size_header
{
	size_t m_size;
};

template<>
size_t bytes_length(const std::string& str)
{
	return sizeof(serial_size_header) + str.size() + 1; // + 1 for null char
}
template<>
void bytes_encode(const std::string& str, char* buffer)
{
	serial_size_header header;
	header.m_size = str.size();
	meta<serial_size_header>().encode(header, &buffer);
	
	memcpy(buffer, str.c_str(), str.size());
	buffer[str.size()] = '\0'; // see + 1 in length
}
template<>
void bytes_decode(std::string& str, const char* buffer)
{
	serial_size_header header;
	header.m_size = str.size();
	meta<serial_size_header>().encode(header, &buffer);

	str = std::string(buffer, buffer + size);
}
// end string serializer

// vector serializer

template<>
size_t bytes_length(const std::vector<int>& vec)
{
	size_t length = sizeof(serial_size_header);
	for (const int& item : vec)
	{
		length += meta<int>().length(item);
	}

	return length;
}
template<>
void bytes_encode(const std::vector<int>& vec, char* buffer)
{
	serial_size_header header;
	header.m_size = vec.size();
	meta<serial_size_header>().encode(header, &buffer);

	for (const int& item : vec)
	{
		meta<int>().encode(item, &buffer);
	}
}
template<>
void bytes_decode(std::vector<int>& vec, const char* buffer, size_t size)
{
	serial_size_header header;
	meta<serial_size_header>().decode(&header, &buffer);
	vec.resize(header.m_size);

	for (int& item : vec)
	{
		meta<int>().decode(&item, &buffer);
	}
}
// end vector serializer

int main()
{
	run_test(test_free_list_mark);
	run_test(test_free_list_unmark);
	run_test(test_free_list_helpers);

	// asset_manager asset;
	// asset.set_loader<int>("int", load_int);
	// asset.set_loader<texture>("png", load_texture);
	// asset.set_loader<texture>("jpg", load_texture);

	// asset.set_loader<char>("bin", load_bytes);
	
	// int a = asset.load("10.int");
	// char* bytes = asset.load("500.bin");

	// char* bytes2 = asset.load("");

	// texture t = asset.load("example.png");

	//asset<texture> tex = asset.load<texture>("example.png");

	// int* i_ptr = 0;

	// asset<int> i(i_ptr);
	// i += 1;

	entity e  = entities().create<int, std::string>();
	entity e1 = entities().create<int, float, std::string>();

	e.set<int>(0xCABCDEFC);
	e1.set<int>(0xCABCDEFC);
	e1.set<float>(1024.1024f);

	e.set<std::string>("this is a test of serializing a string");
	e1.set<std::string>("another test is so nice :)");

	std::vector<std::pair<char*, char*>> chunks;

	for (const auto& [_, store] : entities().m_storage)
	{
		size_t total_length = 0;

		for (pool_iterator itr(store.m_pool); itr.more(); itr.next())
		{
			entity_data* entity = itr.get<entity_data>();
			for (const component& component : store.m_archetype.m_components)
			{
				void* data = store.get_raw_pointer(store.wrap(entity), component);
				total_length += component.m_length(data);
			}
		}

		char* chunk = new char[total_length];
		char* chunk_itr = chunk;

		for (pool_iterator itr(store.m_pool); itr.more(); itr.next())
		{
			entity_data* entity = itr.get<entity_data>();
			for (const component& component : store.m_archetype.m_components)
			{
				void* data = store.get_raw_pointer(store.wrap(entity), component);
				component.m_encode(data, &chunk_itr);
			}
		}

		chunks.emplace_back(chunk, chunk_itr);
	}

	for (auto [begin, end] : chunks)
	{
		__debug_print_memory(begin, 16, 2);
	}


	std::vector<int> ints = {12, 12, 432, 345, 43, 1, 2, 3};

	char* ints_bytes = meta<std::vector<int>>().encode(ints);

	printf("\n");
	__debug_print_memory(ints_bytes, 16, 8);

	std::vector<int> ints_decode;
	meta<std::vector<int>>().decode(&ints_decode, ints_bytes);

	return 0;
}
