#pragma once

#include "iw/engine/System.h"
#include <vector>

#include "iw/common/Components/Transform.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/ParticleSystem.h"

namespace iw {
namespace Editor {
	class SpaceInspectorSystem
		: public SystemBase
	{
	private:
		std::vector<EntityHandle> m_entities;
		float m_timer;

		EntityHandle m_selectedEntity;
		ref<MeshData> m_selectedMeshData;

	public:
		SpaceInspectorSystem();

		void Update() override;
		void ImGui()  override;
	private:
		void AddEntityToHierarchy(
			EntityHandle& handle,
			iw::Transform* cache);

		void PrintTransform(const iw::Transform* transform);
		void PrintMesh(const iw::Mesh* mesh);
		void PrintModel(const iw::Model* model);
		void PrintParticleSystem(const iw::ParticleSystem<>* system);
	};
}

	using namespace Editor;
}
