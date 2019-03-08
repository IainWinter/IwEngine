#pragma once

#include "Core.h"
#include <vector>
#include <string>
#include "iw\logger.h"

namespace IwEngine {
	class Subspace {
	private:
		std::vector<Subspace*> m_subspaces;
		std::string name;

	public:
		Subspace(const char* name)
			: name(name)
		{}

		void Update() {
			for (Subspace* s : m_subspaces) {
				s->Update();
			}

			Tick();
		}

	protected:
		void Tick() {
			LOG_INFO << "Updated " << name;
		}
	};

	class Space {
	private:
		std::vector<Subspace*> m_subspaces;

	public:
		void MakeSubspace(const char* name) {
			m_subspaces.push_back(new Subspace(name));
		}

		void Update() {
			for (Subspace* s : m_subspaces) {
				s->Update();
			}
		}
	};
}
