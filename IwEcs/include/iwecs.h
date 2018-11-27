#pragma once

#ifdef IWECS_EXPORT
#	define IWECS_API __declspec(dllexport)
#else
#	define IWECS_API __declspec(dllimport)
#endif

#include <vector>

namespace iwecs {
	using entity_t = std::size_t;

	class entity_component_data {
	private:
		std::vector<const void*> m_components;
	public:
		entity_component_data() {}

		entity_component_data(std::size_t index, std::size_t size, void** components) {
			for (std::size_t i = 0; i < size; i++) {
				m_components.push_back(components[i]);
			}
		}
	};

	class entity_data {
	private:
		entity_component_data m_components;
		std::size_t m_index;
	public:
		entity_data()
			: m_index(0) {}

		entity_data(std::size_t index, entity_component_data components) {
			m_index = index;
			m_components = components;
		}
	};
}