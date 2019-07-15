#include "iw/math/vector2.h"
#include "iw/physics/2D/Collision/AABB2D.h"

namespace IwPhysics {
	template<
		typename T>
	class Quadtree {
	public:
	private:
		struct m_Item {
			iwm::vector2 Position;
			AABB2D Bounds;
			T* Data;
		};

		static constexpr float _Infinity
			= std::numeric_limits<float>::infinity();

		m_Item m_items[8];
		AABB2D m_bounds;
		std::size_t m_size;
		std::size_t m_height;

		Quadtree<T>* m_northEast;
		Quadtree<T>* m_northWest;
		Quadtree<T>* m_southEast;
		Quadtree<T>* m_southWest;

	public:
		Quadtree(
			std::size_t height = 0)
			: m_bounds(AABB2D(iwm::vector2::zero, _Infinity))
			, m_size(0)
			, m_height(height)
		{}

		int Insert(
			const T& item,
			const AABB2D& bounds,
			const iwm::vector2& position)
		{

		}

		void Erase(
			const int proxy)
		{

		}

		void Move(
			const int proxy,
			const iwm::vector2& displacement)
		{

		}

		inline const AABB2D& Bounds() {
			return m_bounds;
		}

		inline std::size_t Size() {
			return m_size;
		}

		inline std::size_t Height() {
			return m_height;
		}

		std::size_t Capacity() {

		}
	private:
	};
}
