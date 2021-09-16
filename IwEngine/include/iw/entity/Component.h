#pragma once

#include "IwEntity.h"
#include <initializer_list>
#include <functional>
#include <typeindex>
#include <vector>

namespace iw {
namespace ECS {
	using func_DeepCopy   = std::function<void(void*, void*)>;
	using func_Destructor = std::function<void(void*)>;

	struct Component {
#ifdef IW_USE_REFLECTION
		const Type* Type = nullptr;
#else
		size_t Type = 0u;
#endif
		size_t Size = 0u;
		const char* Name = nullptr;

		size_t Id = 0u;

		func_DeepCopy DeepCopyFunc;
		func_Destructor DestructorFunc;

		static inline size_t Hash(
			std::initializer_list<ref<Component>> components)
		{
			return Hash(components.begin(), components.end());
		}

		template<
			typename _itr>
		static size_t Hash(
			_itr begin,
			_itr end)
		{
			size_t seed = std::distance(begin, end);
			for (_itr component = begin; component != end; component++) {
				seed ^= (size_t)(*component)->Name + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}

			return seed;
		}
	};

	struct ComponentQuery {
	private:
		std::vector<ref<Component>> m_all;
		std::vector<ref<Component>> m_any;
		std::vector<ref<Component>> m_none;

	public:
		void SetAll(
			std::initializer_list<ref<Component>> components)
		{
			m_all = components;
		}

		void SetAny(
			std::initializer_list<ref<Component>> components)
		{
			m_any = components;
		}

		void SetNone(
			std::initializer_list<ref<Component>> components)
		{
			m_none = components;
		}

		const auto& GetAll()  const { return m_all; }
		const auto& GetAny()  const { return m_any; }
		const auto& GetNone() const { return m_none; }

		const auto GetComponents() const {
			std::vector<ref<Component>> unique = GetAll();

			for (const auto& c : GetAny()) {
				if (std::find(unique.begin(), unique.end(), c) == unique.end()) {
					unique.push_back(c);
				}
			}

			for (const auto& c : GetNone()) { 
				auto itr = std::find(unique.begin(), unique.end(), c);
				if (itr != unique.end()) {
					unique.erase(itr);
				}
			}

			return unique;
		}
	};

	template<
		typename _t>
	func_DeepCopy GetCopyFunc() {
		return [](void* ptr, void* data) {
			_t* p = (_t*)ptr;
			_t* d = (_t*)data;

			if (!d) {
				new (p) _t(); // :(
			}

			else {
				new(p) _t(*d);
			}
		};
	}

	template<
		typename _t>
	func_Destructor GetDestructorFunc() {
		return [](void* ptr) {
			((_t*)ptr)->~_t();
		};
	}
}

	using namespace ECS;
}
