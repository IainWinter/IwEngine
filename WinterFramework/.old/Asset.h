#pragma once

// really cant find a pattern I like

// goal of this
// loading assets is annoying and really you want to be able to store them in a local var, but that can get annoying too because now you have extra state to worry about
// to fix the issue of having code that looks like

// Asset->Load<Namespace::TypeName>("a/long/path/to/an/asset.txt");

// because every asset has unique file formats, we could just search through the folders when loading for the first time, the store just the name into a hashmap

// struct IAssetCache
// {
// 	virtual ~IAssetCache() {};
// };

// template<typename _t>
// struct AssetCache : IAssetCache
// {
// private:
// 	std::unordered_map<std::string, r<_t>> m_loaded;
// 	std::string m_root;
// public:

// 	AssetCache(const std::string& root) : m_root(root) {}

// 	std::string FullPath(const std::string& name)
// 	{
// 		return m_root + name; // copy :(
// 	}

// //protected:
// 	template<typename... _args>
// 	r<_t> Create(const std::string& name, const _args&... args)
// 	{
// 		r<_t> a = std::make_shared<_t>(args...);
// 		m_loaded[name] = a;
// 		return a;
// 	}

// 	r<_t> GetLoaded(const std::string& name)
// 	{
// 		return m_loaded.at(name);
// 	}

// 	bool IsLoaded(const std::string& name)
// 	{
// 		return m_loaded.find(name) != m_loaded.end();
// 	}
// };

// Idea for async loading through events, too complex for this simple examples

// struct AssetStore
// {
// 	std::vector<IAssetCache*> m_storage;

// 	void RegsiterLoader(IAssetCache* loader, const std::vector<std::string>& forTheseExtensions)
// 	{
// 		events().attach<event_LoadAsset>(loader).only_if(
// 			[forTheseExtensions](const event_LoadAsset& e)
// 			{ 
// 				for (const std::string& ext : forTheseExtensions) // explore faster ways to do this, a set would be much faster...
// 				{
// 					if (strcmp(e.ext(), ext.c_str()) == 0) return true;
// 				}
// 				return false;
// 			});

// 		m_storage.push_back(loader);
// 	}

// 	void UnregisterLoader(IAssetCache* loader)
// 	{
// 		events().detach(loader);
// 	}

// 	~AssetStore()
// 	{
// 		for (IAssetCache* store : m_storage)
// 		{
// 			delete store;
// 		}
// 	}
// };

// #include "Event.h"

// struct event_LoadAsset
// {
// 	std::string path;

// 	const char* ext() const
// 	{
// 		return path.c_str() + path.find_last_of('.');
// 	}
// };

// template<typename _t>
// struct event_AssetLoaded
// {
// 	r<_t> Loaded;
// };

// #include <memory>
// #include <mutex>
// #include <chrono>

// // Take this example code.
// // we state that we are loading a texture in three ways
// // 1. the type of t
// // 2. the template in load
// // 3. the file 
// // texture t = asset.load<texture>("example.png");

// // acts as a shared ptr and a future together
// // asset_manager will load data async, then ping this guy
// // template<typename _t>
// // struct asset
// // {
// // 	asset(
// // 		std::shared_ptr<_t>& data
// // 	)
// // 		: m_data (data)
// // 	{
// // 		m_meta_request_time = 
// // 	}

// // private:
// // 	std::shared_ptr<_t> m_data;
	
// // 	std::mutex m_meta_mutex;
// // 	float m_meta_request_time;
// // 	bool m_meta_is_done_loading;

// // 	// std::mutex m_load_mutex;
// // 	//float m_load_precent; // for loading screens
// // 	//bool m_is_ready;
// // 	//int m_access
// // };

// // #include <unordered_map>
// // #include "Common.h"

// // template<typename _t, typename... _args>
// // _t create_asset(const _args&... args)
// // {
// // 	static_assert("no loader for type");
// // }

// // struct asset_manager
// // {
// // 	std::unordered_map<size_t, r<void>> m_assets;

// // 	template<typename _t, typename... _args>
// // 	r<_t> load(const _args&... args)
// // 	{
// // 		r<_t> asset = create_asset<_t>(args...);
// // 		m_assets.emplace(typeid(_t).hash_code(), asset);
// // 		return asset;
// // 	}
// // };

// #include <string>
// #include <functional>
// #include <unordered_map>
// #include "util/pool_allocator.h"
// #include <sys/types.h>
// #include <filesystem>
// #include <iostream>

// struct FileName
// {
// 	std::string m_directory;
// 	std::string m_name;
// 	std::string m_ext;

// 	std::string full_name() const { return m_directory + m_name + m_ext; }
// 	std::string name_ext() const {return m_name + m_ext; };
// };

// struct AssetStore
// {
// public:
// 	pool_allocator m_pool;
// 	std::unordered_map<std::string, void*> m_loaded;
// private:
// 	std::function<void*(const FileName&, allocator*)> m_load;
// 	std::function<void(void*, allocator*)> m_free;

// public:
// 	AssetStore() = default;

// 	AssetStore(
// 		size_t item_size,
// 		std::function<void*(const FileName&, allocator*)> load,
// 		std::function<void(void*, allocator*)> free
// 	)
// 		: m_pool (item_size)
// 		, m_load (load)
// 		, m_free (free) 
// 	{}

// 	void* Load(const FileName& file)
// 	{
// 		assert(!!m_load && "asset_store has no loading function");
// 		void* ptr = m_load(file, &m_pool);
// 		m_loaded[file.full_name()] = ptr;
// 		return ptr;
// 	}

// 	void Free(const FileName& file)
// 	{
// 		assert(!!m_free && "asset_store has no free function");
// 		return m_free(m_loaded[file.full_name()], &m_pool);
// 	}
// };

// struct AssetManager
// {
// 	std::string m_root;
// 	std::unordered_map<std::string, FileName> m_dir_cache; // name.ext -> path/name.ext
// 	std::unordered_map<std::string, AssetStore> m_storage;

// 	AssetManager(
// 		const std::string& name
// 	)
// 		: m_root (name)
// 	{}

// 	void UpdateDirCache()
// 	{
// 		m_dir_cache.clear();
// 		for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(m_root))
// 		{
// 			std::cout << dir_entry << '\n';
// 			if (dir_entry.is_regular_file())
// 			{
//  				std::string full = dir_entry.path().string();

// 				size_t dot   = full.find_last_of('.');
// 				size_t slash = full.find_last_of('/');

// 				if (slash > full.size()) slash = 0;

// 				std::string ext  = full.substr(dot);
// 				std::string name = full.substr(slash + 1, dot - slash - 1);
// 				std::string path = full.substr(0, slash + 1);

// 				FileName file = { path, name, ext };

// 				auto itr = m_dir_cache.find(file.name_ext());
// 				if (itr == m_dir_cache.end())
// 				{
// 					m_dir_cache.emplace(file.name_ext(), file);
// 				}

// 				else
// 				{
// 					if (m_dir_cache.size() > 1)
// 					{
// 						printf("[ASSET] asset_manager::update_dir_cache Found more than 1 match, using first...");
// 						printf("\n\t\t Using. %s", itr->second.full_name());
// 						printf("\n\t\t Other. %s", full);
// 					}
// 				}
// 			}
// 		}
// 	}

// 	const FileName& FindFile(const std::string& name)
// 	{
// 		size_t dot = name.find_last_of('.');
// 		assert(dot != sizeof(-1) && "asset name needs extension");

// 		auto itr = m_dir_cache.find(name);
// 		if (itr == m_dir_cache.end())
// 		{
// 			itr = m_dir_cache.emplace(name, FileName {"", name.substr(0, dot), name.substr(name.size() - dot - 1)}).first;
// 		}

// 		return itr->second;
// 	}

// 	template<typename _t>
// 	_t& Load(const std::string& name)
// 	{
// 		const FileName& file = FindFile(name);
// 		return *(_t*)m_storage.at(file.m_ext).Load(file);
// 	}

// 	template<typename _t>
// 	void SetLoader(std::function<void(const FileName&, _t*)>&& load, const std::vector<std::string>& exts)
// 	{
// 		for (const std::string& ext : exts)
// 		{
// 			m_storage[ext] = AssetStore(
// 				sizeof(_t),
// 				[load](const FileName& file, allocator* alloc) { _t* ptr = alloc->alloc<_t>(); load(file, ptr); return ptr; },
// 				[]    (void* ptr,            allocator* alloc) { alloc->free<_t>((_t*)ptr, 1); }
// 			);
// 		}
// 	}
// };