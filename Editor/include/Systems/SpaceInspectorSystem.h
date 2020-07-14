#pragma once

#include "iw/engine/System.h"
#include <vector>

#include "iw/common/Components/Transform.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/ParticleSystem.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Dynamics/Rigidbody.h"

#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/CapsuleCollider.h"

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

		template<typename _t>
		void PrintPlaneCollider(
			iw::Physics::impl::PlaneCollider<_t>* collider)
		{
			if (!collider) return;

			if (ImGui::BeginTable("Plane Collider", 2)) {
				ImGui::TableSetupColumn("", 0, 0.3f);
				ImGui::TableSetupColumn("", 0, 0.7f);

				PrintCell("Plane");    PrintEditCell(&collider->Plane.P);
				PrintCell("Distance"); PrintEditCell(&collider->Plane.D);

				ImGui::EndTable();
			}
		}

		template<typename _t>
		void PrintSphereCollider(
			iw::Physics::impl::SphereCollider<_t>* collider)
		{
			if (!collider) return;

			if (ImGui::BeginTable("Sphere Collider", 2)) {
				ImGui::TableSetupColumn("", 0, 0.3f);
				ImGui::TableSetupColumn("", 0, 0.7f);

				PrintCell("Center"); PrintEditCell(&collider->Center);
				PrintCell("Radius"); PrintEditCell(&collider->Radius);

				ImGui::EndTable();
			}
		}

		template<typename _t>
		void PrintCapsuleCollider(
			iw::Physics::impl::CapsuleCollider<_t>* collider)
		{
			if (!collider) return;

			if (ImGui::BeginTable("Capsule Collider", 2)) {
				ImGui::TableSetupColumn("", 0, 0.3f);
				ImGui::TableSetupColumn("", 0, 0.7f);

				vector3 axis     = collider->Direction;
				vector3 lastAxis = axis;

				PrintCell("Center"); PrintEditCell(&collider->Center);
				PrintCell("Height"); PrintEditCell(&collider->Height);
				PrintCell("Radius"); PrintEditCell(&collider->Radius);

				PrintCell("Direction");

				bool x = axis.x;
				bool y = axis.y;
				bool z = axis.z;

				ImGui::TableNextCell();
				ImGui::Checkbox("X", &x); ImGui::SameLine();
				ImGui::Checkbox("Y", &y); ImGui::SameLine();
				ImGui::Checkbox("Z", &z);

				if (x || y || z) {
					axis = vector3(x, y, z);

					if (axis.x != lastAxis.x) {
						axis.y = 0;
						axis.z = 0;
					}

					else if (axis.y != lastAxis.y) {
						axis.x = 0;
						axis.z = 0;
					}

					else if (axis.z != lastAxis.z) {
						axis.x = 0;
						axis.y = 0;
					}

					collider->Direction = axis;
				}

				ImGui::EndTable();
			}
		}

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

			     if constexpr (std::is_same_v<float, _t>)   ImGui::DragFloat ("", (float*)value, 0.1f);
			else if constexpr (std::is_same_v<bool,  _t>)   ImGui::Checkbox  ("", (bool*) value);
			else if constexpr (std::is_same_v<vector2, _t>) ImGui::DragFloat2("", (float*)value, 0.1f);
			else if constexpr (std::is_same_v<vector3, _t>) ImGui::DragFloat3("", (float*)value, 0.1f);
			else if constexpr (std::is_same_v<vector4, _t>
				           || std::is_same_v<quaternion, _t>) ImGui::DragFloat4("", (float*)value, 0.1f);

			ImGui::PopID();
		}
	};
}

	using namespace Editor;
}
