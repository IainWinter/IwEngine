#pragma once

#include "registry.h"

template<typename... _t>
class view {
public:
	template<typename _s>
	class view_bind {
	public:
		std::vector<_s>::iterator begin() {};
		std::vector<_s>::iterator end() {};
	};

	template<typename _s>
	view_bind<_s> bind() {
		return view_bind<_s>();
	}
};

namespace iwecs {
	class isystem {
	public:
		virtual ~isystem() {}

		virtual void update() = 0;
	};

	template<typename... _components_t>
	class system : public isystem {
	private:
		registry& m_registry;
	public:
		virtual ~system() {}

		void update() override {
			view<_components_t...> view = m_registry.view<_components_t...>();
			for (auto& e : view) {
				update();
			}
		}
	protected:
		virtual void update (_components_t&... components) = 0;
	};
}