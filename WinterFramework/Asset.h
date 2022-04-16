#pragma once

// goal of this
// loading assets is annoying and really you want to be able to store them in a local var, but that can get annoying too because now you have extra state to worry about
// to fix the issue of having code that looks like

// Asset->Load<Namespace::TypeName>("a/long/path/to/an/asset.txt");

// because every asset has unique file formats, we could just search through the folders when loading for the first time, the store just the name into a hashmap

#include <memory>
#include <mutex>
#include <chrono>

// Take this example code.
// we state that we are loading a texture in three ways
// 1. the type of t
// 2. the template in load
// 3. the file 
// texture t = asset.load<texture>("example.png");

// acts as a shared ptr and a future together
// asset_manager will load data async, then ping this guy
// template<typename _t>
// struct asset
// {
// 	asset(
// 		std::shared_ptr<_t>& data
// 	)
// 		: m_data (data)
// 	{
// 		m_meta_request_time = 
// 	}

// private:
// 	std::shared_ptr<_t> m_data;
	
// 	std::mutex m_meta_mutex;
// 	float m_meta_request_time;
// 	bool m_meta_is_done_loading;

// 	// std::mutex m_load_mutex;
// 	//float m_load_precent; // for loading screens
// 	//bool m_is_ready;
// 	//int m_access
// };

#include <string>
#include <functional>
#include <unordered_map>
#include "util/pool_allocator.h"
#include <sys/types.h>
#include <filesystem>
#include <iostream>

struct asset_wrap
{
	void* m_asset;
	
	template<typename _t>
	operator _t&()
	{
		return *(_t*)m_asset;
	}
};

struct file_name
{
	std::string m_directory;
	std::string m_name;
	std::string m_ext;

	std::string full_name() const { return m_directory + m_name + m_ext; }
	std::string name_ext() const {return m_name + "." + m_ext; };
};

template<typename _t>
struct loads
{
	virtual void load(const file_name& file, _t* ptr) = 0;
	virtual void free(_t* ptr) = 0;
};

struct asset_store
{
public:
	pool_allocator m_pool;
	std::unordered_map<std::string, void*> m_loaded;
private:
	std::function<void*(const file_name&, allocator*)> m_load;
	std::function<void(void*, allocator*)> m_free;

public:
	asset_store() = default;

	asset_store(
		size_t item_size,
		std::function<void*(const file_name&, allocator*)> load,
		std::function<void(void*, allocator*)> free
	)
		: m_pool (item_size)
		, m_load (load)
		, m_free (free) 
	{}

	asset_store(
		size_t item_size,
		std::function<void*(const file_name&, allocator*)> load
	)
		: m_pool (item_size)
		, m_load (load)
	{}

	void* load(const file_name& file)
	{
		assert(!!m_load && "asset_store has no loading function");
		void* ptr = m_load(file, &m_pool);
		m_loaded[file.full_name()] = ptr;
		return ptr;
	}

	void free(const file_name& file)
	{
		assert(!!m_free && "asset_store has no free function");
		return m_free(m_loaded[file.full_name()], &m_pool);
	}
};

struct asset_manager
{
	// root search folder and a cache of results
	std::string m_root;
	std::unordered_map<std::string, file_name> m_dir_cache; // name.ext -> path/name.ext

	std::unordered_map<std::string, asset_store> m_storage;

	void update_dir_cache()
	{
		m_dir_cache.clear();
		for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(m_root))
		{
			std::cout << dir_entry << '\n';
			if (dir_entry.is_regular_file())
			{
 				std::string full = dir_entry.path().string();

				size_t dot   = full.find_last_of('.');
				size_t slash = full.find_last_of('/');

				if (slash > full.size()) slash = 0;

				std::string ext  = full.substr(full.size() - dot - 1);
				std::string name = full.substr(slash, dot - slash);
				std::string path = full.substr(0, slash);

				file_name file = {path, name, ext};

				auto itr = m_dir_cache.find(file.name_ext());
				if (itr == m_dir_cache.end())
				{
					m_dir_cache.emplace(file.name_ext(), file);
				}

				else
				{
					if (m_dir_cache.size() > 1)
					{
						printf("[ASSET] asset_manager::update_dir_cache Found more than 1 match, using first...");
						printf("\n\t\t Using. %s", itr->second.full_name());
						printf("\n\t\t Other. %s", full);
					}
				}
			}
		}
	}

	const file_name& find_file(std::string name)
	{
		size_t dot = name.find_last_of('.');
		assert(dot != sizeof(-1) && "asset name needs extension");

		auto itr = m_dir_cache.find(name);
		if (itr == m_dir_cache.end())
		{
			itr = m_dir_cache.emplace(name, file_name {"", name.substr(0, dot), name.substr(name.size() - dot - 1)}).first;
		}

		return itr->second;
	}

	asset_wrap load(const std::string& name)
	{
		const file_name& file = find_file(name);
		return asset_wrap { m_storage.at(file.m_ext).load(file) };
	}

	template<typename _t>
	void set_loader(const std::string& ext, std::function<void(const file_name&, _t*)>&& load)
	{
		m_storage[ext] = asset_store(
			sizeof(_t),
			[load](const file_name& file, allocator* alloc) { _t* ptr = alloc->alloc<_t>(); load(file, ptr); return ptr; },
			[]    (void* ptr,             allocator* alloc) { alloc->free<_t>((_t*)ptr, 1); }
		);
	}

	template<typename _t>
	void set_loader(const std::string& ext, std::function<void*(const file_name&, allocator*)>&& load)
	{
		m_storage[ext] = asset_store(
			sizeof(_t),
			load
		);
	}
};