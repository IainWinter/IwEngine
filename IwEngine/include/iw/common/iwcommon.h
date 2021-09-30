#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWCOMMON_DLL
#		define IWCOMMON_API __declspec(dllexport)
#else
#		define IWCOMMON_API __declspec(dllimport)
#	endif
#else
#	define IWCOMMON_API
#endif

#include "iw/util/reflection/ReflectDef.h"

#pragma warning (push)
#pragma warning (disable: 26812) // non enum classes, they are already namespaced so why does it matter

#include "iw/math/matrix.h"
#include <vector>

namespace iw {
#ifdef IW_DOUBLE_PRECISION
	using scalar = long double;
#else
	using scalar = float;
#endif

	enum /*class*/ Dimension {
		d2 = 2,
		d3 = 3,
	};

	template<Dimension _d>
	using _vec = glm::vec<size_t(_d), scalar>;

	template<Dimension _d>
	using _ivec = glm::vec<size_t(_d), int>;

namespace math_translation {
	inline vec<3> from_glm(glm::vec3 v) {
		vec<3> result;

		result.set(0, v.x);
		result.set(1, v.y);
		result.set(2, v.z);

		return result;
	}

	inline mat<3, 3> from_glm(glm::mat3 v) {
		mat<3, 3> result;

		for (int i = 0; i < 9; i++) result.elements[i] = v[i / 3][i % 3];

		return result;
	}

	inline glm::vec3 to_glm(vec<3> vec) {
		return glm::vec3(vec.get(0), vec.get(1), vec.get(2));
	}

	inline glm::vec2 to_glm(vec<2> vec) {
		return glm::vec2(vec.get(0), vec.get(1));
	}

	inline float cross_length(glm::vec2 a, glm::vec2 b) { // should move to geom 2d
		return a.x * b.y - a.y * b.x;
	}
}

namespace helpers {
	template<
		typename _t,
		typename _container>
	bool contains(const _container& container, const _t& value)
	{
		return std::find(container.begin(), container.end(), value)
			!= container.end();
	} // needs specialization for maps

	template<
		typename _t>
	_t choose(
		std::vector<std::pair<_t, float>>& item_weights) // need to template list for const/nonconst 
	{
		float total_weight = 0.f;
		for (const auto& [item, weight] : item_weights)
		{
			total_weight += weight;
		}

		float pick = randf() * total_weight;
				
		float last_weight = 0.f;
		for (const auto& [item, weight] : item_weights)
		{
			if (pick > last_weight && pick < weight + last_weight)
			{
				return item;
			}

			last_weight += weight;
		}

		return item_weights.back().first;
	}

	template<
		typename _t>
	_t& choose_e(
		std::vector<_t>& items) // need to template list for const/nonconst 
	{
		if (items.size() == 0)
		{
			throw std::invalid_argument("");
		}

		return items.at(iw::randi(items.size() - 1));
	}

	template<
		typename _t>
	std::pair<_t, _t> xy(const _t& index, const _t& width)
	{
		return {
			index % width,
			index / width
		};
	}
}
	using namespace helpers;
	using namespace math_translation;
}
