#pragma once

#include "Mesh.h"
#include <vector>

namespace iw {
namespace Graphics {
	struct Model {
	private:
		std::vector<Mesh> m_meshes;

	public:
		void AddMesh(
			Mesh& mesh)
		{
			m_meshes.push_back(mesh);
		}

		/*void RemoveMesh(Mesh& mesh) {
			m_meshes,era(mesh);
		}*/

		std::vector<Mesh>::iterator begin(){
			return m_meshes.begin();
		}

		std::vector<Mesh>::iterator end() {
			return m_meshes.end();
		}

		Mesh& GetMesh(
			unsigned index)
		{
			return m_meshes.at(index);
		}

		unsigned MeshCount() const {
			return m_meshes.size();
		}
	};
}

	using namespace Graphics;
}
