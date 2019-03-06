#pragma once

#include "Core.h"
#include <vector>
#include <string>
#include "logger.h"

namespace IwEngine {
	class Space {
	private:
		std::vector<Space*> m_subspaces;
		std::string name;

	public:
		Space(const char* name)
			: name(name)
		{}

		void Make_Subspace(const char* name) {
			m_subspaces.push_back(new Space(name));
		}

		void Update() {
			for (Space* s : m_subspaces) {
				s->Update();
			}

			Tick();
		}

		void Tick() {
			LOG_INFO << "Updating " << name;
		}
	};
}
