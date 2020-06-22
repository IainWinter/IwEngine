#pragma once

#include "iw/engine/System.h"
#include <vector>

#include "iw/common/Components/Transform.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/ParticleSystem.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Dynamics/Rigidbody.h"

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

		void PrintTransform(iw::Transform* transform, bool showParent = true);
		void PrintMesh(iw::Mesh* mesh);
		void PrintModel(iw::Model* model);
		void PrintParticleSystem(iw::ParticleSystem<>* system);
		void PrintCollisionObject(iw::CollisionObject* object);
		void PrintRigidbody(iw::Rigidbody* body);

		template<
			typename _t>
		void PrintCell(_t t)
		{
			std::stringstream ss;
			ss << t;

			ImGui::TableNextCell();
			ImGui::Text(ss.str().c_str());
		}

		template<
			typename _t>
		void PrintHeader(_t t)
		{
			std::stringstream ss;
			ss << t;

			//ImGui::TableNextCell();
			ImGui::TableSetupColumn(ss.str().c_str());
		}

		template<
			typename _t>
		void PrintEditCell(
			_t* value)
		{
			ImGui::TableNextCell();
			ImGui::PushID((void*)value);

			     if constexpr (std::is_same_v<float, _t>)   ImGui::DragFloat ("", (float*)value);
			else if constexpr (std::is_same_v<bool,  _t>)   ImGui::Checkbox  ("", (bool*) value);
			else if constexpr (std::is_same_v<vector2, _t>) ImGui::DragFloat2("", (float*)value);
			else if constexpr (std::is_same_v<vector3, _t>) ImGui::DragFloat3("", (float*)value);
			else if constexpr (std::is_same_v<vector4, _t>
				           || std::is_same_v<quaternion, _t>) ImGui::DragFloat4("", (float*)value);

			ImGui::PopID();
		}
	};
}

	using namespace Editor;
}
