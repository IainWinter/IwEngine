#pragma once

#include "Mesh.h"
#include "iw/common/Components/Transform.h"
#include <vector>

namespace iw {
namespace Graphics {
	struct Model {
	private:
		std::vector<Mesh>      m_meshes;
		std::vector<Transform> m_transforms;

	public:
		~Model() = default;

		void AddMesh(
			Mesh& mesh,
			Transform& transform = Transform())
		{
			m_meshes.push_back(mesh);
			m_transforms.push_back(transform);
		}

		/*void RemoveMesh(Mesh& mesh) {
			m_meshes,era(mesh);
		}*/

		std::vector<Mesh>& GetMeshes() {
			return m_meshes;
		}

		Mesh& GetMesh(
			unsigned index)
		{
			return m_meshes.at(index);
		}

		Mesh& GetMesh(
			std::string name)
		{
			for (Mesh& mesh : m_meshes) {
				if (mesh.Data()->Name() == mesh.Data()->Name()) {
					return  mesh;
				}
			}

			return Mesh();
		}

		Transform& GetTransform(
			unsigned index)
		{
			return m_transforms.at(index);
		}

		unsigned MeshCount() const {
			return m_meshes.size();
		}
	};
}

	using namespace Graphics;
}
