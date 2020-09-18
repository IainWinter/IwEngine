#include "Systems/SpaceInspectorSystem.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"

#include "..\imgui\src\imgui_demo.cpp"

namespace iw {
namespace Editor {
	ImGuiTableFlags tflags = ImGuiTableFlags_RowBg;

	SpaceInspectorSystem::SpaceInspectorSystem()
		: SystemBase("Space Inspector")
		, m_timer(0.0f)
		, m_selectedEntity()
	{}

	void SpaceInspectorSystem::Update() {
		m_timer += iw::Time::DeltaTime();

		if (m_timer > 1.0f / 60) {
			m_timer = 0;

			m_entities.swap(Space->Entities());
		}
	}

	std::string PrintName(
		EntityHandle handle)
	{
		std::stringstream ss;
		ss << "Entity id: " << handle.Index << " v: " << handle.Version << "\0";
		return ss.str();
	}

	void SpaceInspectorSystem::ImGui() {
		ImGui::Begin("Space Inspector");

		for (EntityHandle handle : m_entities) {
			AddEntityToHierarchy(handle, nullptr);
		}

		ImGui::End();

		ImGui::Begin("Entity Inspector");

		if (m_selectedEntity == EntityHandle::Empty) return;

		if (ImGui::CollapsingHeader("Entity", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::BeginTable("Entity", 2)) {
				ImGui::TableSetupColumn("", 0, 0.3f);
				ImGui::TableSetupColumn("", 0, 0.7f);

				PrintCell("Id");      PrintCell(m_selectedEntity.Index);
				PrintCell("Version"); PrintCell(m_selectedEntity.Version);

				ImGui::EndTable();
			}
		}

		ref<Archetype> archetype = Space->GetArchetype(m_selectedEntity);
		for (unsigned i = 0; i < archetype->Count; i++) {
			ref<Component> component = archetype->Layout[i].Component;

			// shit sucks should be like
			// component->PrintEditor(); or something

			if (component == Space->GetComponent<iw::Transform>()) {
				iw::Transform*       t = Space->FindComponent<iw::Transform>      (m_selectedEntity);
				iw::CollisionObject* c = Space->FindComponent<iw::CollisionObject>(m_selectedEntity);
				iw::Rigidbody*       r = Space->FindComponent<iw::Rigidbody>      (m_selectedEntity);

					 if (c) t = &c->Trans();
				else if (r) t = &r->Trans();

				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintTransform(t);
				}
			}

			else if (component == Space->GetComponent<iw::Mesh>()) {
				if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintMesh(Space->FindComponent<iw::Mesh>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::Model>()) {
				if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintModel(Space->FindComponent<iw::Model>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::ParticleSystem<>>()) {
				if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintParticleSystem(Space->FindComponent<iw::ParticleSystem<>>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::CollisionObject>()) {
				if (ImGui::CollapsingHeader("Collision Object", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintCollisionObject(Space->FindComponent<iw::CollisionObject>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::Rigidbody>()) {
				if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintRigidbody(Space->FindComponent<iw::Rigidbody>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::PlaneCollider>()) {
				if (ImGui::CollapsingHeader("Plane Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintPlaneCollider(Space->FindComponent<iw::PlaneCollider>(m_selectedEntity));
				}
			}

			//else if (component == Space->GetComponent<iw::PlaneCollider2>()) {
			//	if (ImGui::CollapsingHeader("Plane Collider 2D")) {
			//		PrintPlaneCollider(Space->FindComponent<iw::PlaneCollider2>(m_selectedEntity));
			//	}
			//}
			//
			//else if (component == Space->GetComponent<iw::PlaneCollider4>()) {
			//	if (ImGui::CollapsingHeader("Plane Collider 4D")) {
			//		PrintPlaneCollider(Space->FindComponent<iw::PlaneCollider4>(m_selectedEntity));
			//	}
			//}

			else if (component == Space->GetComponent<iw::SphereCollider>()) {
				if (ImGui::CollapsingHeader("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintSphereCollider(Space->FindComponent<iw::SphereCollider>(m_selectedEntity));
				}
			}

			//else if (component == Space->GetComponent<iw::SphereCollider2>()) {
			//	if (ImGui::CollapsingHeader("Sphere Collider 2D")) {
			//		PrintSphereCollider(Space->FindComponent<iw::SphereCollider2>(m_selectedEntity));
			//	}
			//}

			//else if (component == Space->GetComponent<iw::SphereCollider4>()) {
			//	if (ImGui::CollapsingHeader("Sphere Collider 4D")) {
			//		PrintSphereCollider(Space->FindComponent<iw::SphereCollider4>(m_selectedEntity));
			//	}
			//}

			else if (component == Space->GetComponent<iw::CapsuleCollider>()) {
				if (ImGui::CollapsingHeader("Capsule Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintCapsuleCollider(Space->FindComponent<iw::CapsuleCollider>(m_selectedEntity));
				}
			}

			//else if (component == Space->GetComponent<iw::CapsuleCollider2>()) {
			//	if (ImGui::CollapsingHeader("Capsule Collider 2D")) {
			//		PrintCapsuleCollider(Space->FindComponent<iw::CapsuleCollider2>(m_selectedEntity));
			//	}
			//}

			//else if (component == Space->GetComponent<iw::CapsuleCollider4>()) {
			//	if (ImGui::CollapsingHeader("Capsule Collider 4D")) {
			//		PrintCapsuleCollider(Space->FindComponent<iw::CapsuleCollider4>(m_selectedEntity));
			//	}
			//}

			else {
				if (ImGui::CollapsingHeader(component->Name)) {

				}
			}
		}

		ImGui::End();

		ImGui::Begin("Resource Inspector");

		if (m_selectedMeshData) {
			ImGui::Text("Name: ");
			ImGui::SameLine();
			ImGui::Text(m_selectedMeshData->Name().c_str());

			if (ImGui::CollapsingHeader("Buffers", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (ImGui::BeginTable("Buffers", 2, tflags)) {
					PrintHeader("Buffer");
					PrintHeader("Count");

					ImGui::TableAutoHeaders();

					for (bName name : m_selectedMeshData->Description().GetBufferNames()) {
						ImGui::TableNextCell();
						ImGui::Text(getbNameStr(name));

						PrintCell(m_selectedMeshData->GetCount(name));
					}

					ImGui::EndTable();
				}
			}

			if (ImGui::CollapsingHeader("Description", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (ImGui::BeginTable("Description", 4, tflags)) {
					PrintHeader("Buffer");
					PrintHeader("Stride");
					PrintHeader("Instance Stride");
					PrintHeader("Elements");

					ImGui::TableAutoHeaders();

					for (bName name : m_selectedMeshData->Description().GetBufferNames()) {
						ImGui::TableNextCell();
						ImGui::Text(getbNameStr(name));

						PrintCell(m_selectedMeshData->Description().GetBufferLayout(name).GetStride());
						PrintCell(m_selectedMeshData->Description().GetBufferLayout(name).GetInstanceStride());

						ImGui::TableNextCell();

						if (ImGui::BeginTable(getbNameStr(name), 1)) {
							auto elements = m_selectedMeshData->Description().GetBufferLayout(name).GetElements();
							for (unsigned j = 0; j < elements.size(); j++) {
								ImGui::TableNextRow();

								std::stringstream ss;

								switch (elements[j].BType) {
									case BufferLayoutType::FLOAT: {
										ss << "float";
										break;
									}
									case BufferLayoutType::UINT: {
										ss << "unsigned int";
										break;
									}
									case BufferLayoutType::UCHAR: {
										ss << "unsigned char";
										break;
									}
									default: {
										ss << "unknow";
										break;
									}
								}

								ss << "x" << elements[j].Count;

								ImGui::Text(ss.str().c_str());
							}

							ImGui::EndTable();
						}
					}

					ImGui::EndTable();
				}
			}
		}

		ImGui::End();

		ImGui::ShowDemoWindow();
	}

	void SpaceInspectorSystem::AddEntityToHierarchy(
		EntityHandle& handle,
		iw::Transform* cache)
	{
		iw::Transform* transform = cache ? cache : Space->FindComponent<iw::Transform>(handle);

		// No transform or a transform that is in the root & has no children
		if (    !transform
			|| ((cache || !transform->Parent()) && transform->ChildCount() == 0))
		{
			bool selected = m_selectedEntity == handle;
			ImGui::Selectable(PrintName(handle).c_str(), &selected);

			if (selected) {
				m_selectedEntity = handle;
			}

			return;
		}

		// If the transform has a parent skip it because it'l get hit when the parent is iterated
		if (!cache && transform->Parent()) return;

		std::stringstream ss;
		ss << "+" << transform->ChildCount();

		bool open = ImGui::TreeNode(transform, ss.str().c_str());

		ImGui::SameLine();

		bool selected = m_selectedEntity == handle;
		ImGui::Selectable(PrintName(handle).c_str(), &selected);

		if (selected) {
			m_selectedEntity = handle;
		}

		if (open) {
			for (iw::Transform* childTransform : transform->Children()) {
				EntityHandle child = Space->FindEntity(childTransform).Handle;
				if(child == EntityHandle::Empty) {
					ImGui::Text("nae");
					continue;
				}

				AddEntityToHierarchy(child, childTransform);
			}

			ImGui::TreePop();
		}
	}

	void SpaceInspectorSystem::PrintTransform(
		iw::Transform* transform,
		bool showParent)
	{
		if (!transform) return;

		ImGui::PushID(transform);

		if (ImGui::BeginTable("Transform", 2)) {
			ImGui::TableSetupColumn("", 0, 0.3f);
			ImGui::TableSetupColumn("", 0, 0.7f);

			PrintCell("Position"); PrintEditCell(&transform->Position);
			PrintCell("Scale");    PrintEditCell(&transform->Scale);

			vector3 euler = transform->Rotation.euler_angles();

			PrintCell("Rotation"); PrintEditCell(&euler);

			transform->Rotation = iw::quaternion::from_euler_angles(euler);

			if (showParent) {
				EntityHandle parent = Space->FindEntity(transform->Parent()).Handle;

				PrintCell("Parent");

				ImGui::TableNextCell();

				if (parent == EntityHandle::Empty) {
					ImGui::Selectable("None", false);
				}

				else {
					bool selected = m_selectedEntity == parent;
					ImGui::Selectable(PrintName(parent).c_str(), &selected);

					if (selected) {
						m_selectedEntity = parent;
					}
				}
			}

			ImGui::EndTable();
		}

		ImGui::PopID();
	}

	void SpaceInspectorSystem::PrintMesh(
		iw::Mesh* mesh)
	{
		if (!mesh) return;

		ImGui::PushID(mesh);

		bool selected = m_selectedMeshData == mesh->Data();
		ImGui::Text("Mesh data: ");
		ImGui::SameLine();
		ImGui::Selectable(mesh->Data()->Name().c_str(), &selected);

		if (selected) {
			m_selectedMeshData = mesh->Data();
		}

		ImGui::PopID();
	}

	void SpaceInspectorSystem::PrintModel(
		iw::Model* model)
	{
		if (!model) return;

		ImGui::PushID(model);

		if (ImGui::TreeNode("Meshes")) {
			for (unsigned i = 0; i < model->MeshCount(); i++) {
				ImGui::Separator();
				PrintTransform(&model->GetTransform(i));
				PrintMesh     (&model->GetMesh(i));
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void SpaceInspectorSystem::PrintParticleSystem(
		iw::ParticleSystem<>* system)
	{
		if (!system) return;

		ImGui::PushID(system);

		PrintMesh(&system->GetParticleMesh());
		
		if (ImGui::TreeNode("Particles")) {
			for (Particle<StaticParticle>& particle : system->Particles()) {
				ImGui::Separator();
				PrintTransform(&particle.Transform, false);
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void SpaceInspectorSystem::PrintCollisionObject(
		iw::CollisionObject* object)
	{
		if (!object) return;

		ImGui::PushID(object);

		bool isTrigger = object->IsTrigger();
		bool isStatic  = object->IsStatic();
		bool isDynamic = object->IsDynamic();

		if (ImGui::BeginTable("Collision Object", 2)) {
			ImGui::TableSetupColumn("", 0, 0.3f);
			ImGui::TableSetupColumn("", 0, 0.7f);

			PrintCell("Trigger"); PrintEditCell(&isTrigger);
			PrintCell("Static");  PrintEditCell(&isStatic);
			PrintCell("Dynamic"); PrintEditCell(&isDynamic);

			ImGui::EndTable();
		}

		object->SetIsTrigger(isTrigger);
		object->SetIsStatic (isStatic);
		
		ImGui::PopID();
	}

	void SpaceInspectorSystem::PrintRigidbody(
		iw::Rigidbody* body)
	{
		if (!body) return;

		ImGui::PushID(body);

		vector3 gravity         = body->Gravity();
		vector3 force           = body->Force();
		vector3 velocity        = body->Velocity();
		vector3 lock            = body->Lock();
		vector3 isLocked        = body->IsLocked();
		float   mass            = body->Mass();
		bool    takesGravity    = body->TakesGravity();
		bool    simGravity      = body->SimGravity();
		bool    isKinematic     = body->IsKinematic();
		float   staticFriction  = body->StaticFriction();
		float   dynamicFriction = body->DynamicFriction();
		float   restitution     = body->Restitution();

		if (ImGui::BeginTable("Rigidbody", 2)) {
			ImGui::TableSetupColumn("", 0, 0.3f);
			ImGui::TableSetupColumn("", 0, 0.7f);

			PrintCell("Gravity");  PrintEditCell(&gravity);
			PrintCell("Force");    PrintEditCell(&force);
			PrintCell("Velocity"); PrintEditCell(&velocity);
			PrintCell("Lock");     PrintEditCell(&lock);

			PrintCell("Lock axis");

			bool x = isLocked.x;
			bool y = isLocked.y;
			bool z = isLocked.z;

			ImGui::TableNextCell();
			ImGui::Checkbox("X", &x); ImGui::SameLine();
			ImGui::Checkbox("Y", &y); ImGui::SameLine();
			ImGui::Checkbox("Z", &z);

			isLocked = vector3(x, y, z);

			PrintCell("Mass");             PrintEditCell(&mass);
			PrintCell("Takes gravity");    PrintEditCell(&takesGravity);
			PrintCell("Simulate gravity"); PrintEditCell(&simGravity);
			PrintCell("Kinemtaic");        PrintEditCell(&isKinematic);
			PrintCell("Static friction");  PrintEditCell(&staticFriction);
			PrintCell("Dynamic friction"); PrintEditCell(&dynamicFriction);
			PrintCell("Restitution");      PrintEditCell(&restitution);

			ImGui::EndTable();
		}

		body->SetGravity        (gravity);
		body->SetForce          (force);
		body->SetVelocity       (velocity);
		body->SetLock           (lock);
		body->SetIsLocked       (isLocked);
		body->SetMass           (mass);
		body->SetTakesGravity   (takesGravity);
		body->SetSimGravity     (simGravity);
		body->SetIsKinematic    (isKinematic);
		body->SetStaticFriction (staticFriction);
		body->SetDynamicFriction(dynamicFriction);
		body->SetRestitution    (restitution);

		PrintCollisionObject(body);

		ImGui::PopID();
	}
}

	using namespace Editor;
}
