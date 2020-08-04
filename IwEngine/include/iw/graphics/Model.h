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

		const std::vector<Mesh>& GetMeshes() const {
			return m_meshes;
		}

		const Mesh& GetMesh(
			unsigned index) const
		{
			return m_meshes.at(index);
		}

		const Mesh& GetMesh(
			std::string name) const
		{
			for (const Mesh& mesh : m_meshes) {
				if (mesh.Data()->Name() == mesh.Data()->Name()) {
					return  mesh;
				}
			}

			return Mesh();
		}

		const Transform& GetTransform(
			unsigned index) const
		{
			return m_transforms.at(index);
		}

		void RemoveMesh(
			unsigned index)
		{
			m_meshes    .erase(m_meshes    .begin() + index);
			m_transforms.erase(m_transforms.begin() + index);
		}

		unsigned MeshCount() const {
			return m_meshes.size();
		}

		std::vector<Mesh>& GetMeshes() {
			return const_cast<std::vector<Mesh>&>(const_cast<const Model*>(this)->GetMeshes());
		}

		Mesh& GetMesh(
			unsigned index)
		{
			return const_cast<Mesh&>(const_cast<const Model*>(this)->GetMesh(index));
		}

		Mesh& GetMesh(
			std::string name)
		{
			return const_cast<Mesh&>(const_cast<const Model*>(this)->GetMesh(name));
		}

		Transform& GetTransform(
			unsigned index)
		{
			return const_cast<Transform&>(const_cast<const Model*>(this)->GetTransform(index));
		}
	};
}

	using namespace Graphics;
}
