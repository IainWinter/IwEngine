#include "iw/engine/Systems/SpaceInspectorSystem.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"

//#include "../../../extern/imgui/src/imgui_demo.cpp"

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

		Archetype archetype = Space->GetArchetype(m_selectedEntity);
		for (unsigned i = 0; i < archetype.Count; i++) {
			ref<Component> component = archetype.Layout[i].Component;

			// shit sucks should be like
			// component->PrintEditor(); or something

			if (component == Space->GetComponent<iw::Transform>()) {
				iw::Transform*       t = Space->FindComponent<iw::Transform>      (m_selectedEntity);
				iw::CollisionObject* c = Space->FindComponent<iw::CollisionObject>(m_selectedEntity);
				iw::Rigidbody*       r = Space->FindComponent<iw::Rigidbody>      (m_selectedEntity);

					 if (c) t = &c->Transform;
				else if (r) t = &r->Transform;

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

			else if (component == Space->GetComponent<iw::Plane>()) {
				if (ImGui::CollapsingHeader("Plane Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintPlaneCollider(Space->FindComponent<iw::Plane>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::Line>()) {
				if (ImGui::CollapsingHeader("Line Collider 2D", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintPlaneCollider(Space->FindComponent<iw::Line>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::Sphere>()) {
				if (ImGui::CollapsingHeader("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintSphereCollider(Space->FindComponent<iw::Sphere>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::Circle>()) {
				if (ImGui::CollapsingHeader("Circle Collider 2D", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintSphereCollider(Space->FindComponent<iw::Circle>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::Capsule>()) {
				if (ImGui::CollapsingHeader("Capsule Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintCapsuleCollider(Space->FindComponent<iw::Capsule>(m_selectedEntity));
				}
			}

			else if (component == Space->GetComponent<iw::Capsule2>()) {
				if (ImGui::CollapsingHeader("Capsule Collider 2D", ImGuiTreeNodeFlags_DefaultOpen)) {
					PrintCapsuleCollider(Space->FindComponent<iw::Capsule2>(m_selectedEntity));
				}
			}

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
										ss << "unknown";
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

		ImGui::Begin("Physics Inspector");

		ImGui::Text("Count: %i", Physics->CollisionObjects().size());

		for (iw::CollisionObject* o : Physics->CollisionObjects()) {
			iw::Entity e = Space->FindEntity(o);
			if (!e) {
				e = Space->FindEntity<iw::Rigidbody>(o);
			}

			if (e)
			{
				ImGui::Text("Entity id: %i v: %i", e.Index(), e.Version());
			}
		}

		ImGui::End();

		//ImGui::ShowDemoWindow();
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

			glm::vec3 euler = glm::eulerAngles(transform->Rotation);

			PrintCell("Rotation"); PrintEditCell(&euler);

			transform->Rotation = glm::quat(euler);

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

		bool selected = m_selectedMeshData == mesh->Data;
		ImGui::Text("Mesh data: ");
		ImGui::SameLine();
		ImGui::Selectable(mesh->Data->Name().c_str(), &selected);

		if (selected) {
			m_selectedMeshData = mesh->Data;
		}

		ImGui::Text("Material");
		PrintMaterial(mesh->Material.get());

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

	void SpaceInspectorSystem::PrintMaterial(
		iw::Material* material)
	{
		if (!material) return;

		ImGui::PushID(material);

		if (ImGui::BeginTable("Material", 2)) {
			ImGui::TableSetupColumn("", 0, 0.3f);
			ImGui::TableSetupColumn("", 0, 0.7f);

			for (iw::Material::MaterialProperty property : material->Properties()) {
				switch (property.Type) {
					case iw::UniformType::FLOAT: {
						if (property.Name.find("has") != std::string::npos) {
							bool b = *(float*)property.Data == 1.0f ? true : false;

							PrintCell(property.Name); PrintEditCell(&b);
							
							material->Set(property.Name, b ? 1.0f : 0.0f);

							break;
						}

						switch (property.Stride) {
							case 1: PrintCell(property.Name); PrintEditCell((float		*)property.Data); break;
							case 2: PrintCell(property.Name); PrintEditCell((glm::vec2*)property.Data, 0, 1, 0.01f); break;
							case 3: PrintCell(property.Name); PrintEditCell((glm::vec3*)property.Data, 0, 1, 0.01f); break;
							case 4: PrintCell(property.Name); PrintEditCell((glm::vec4*)property.Data, 0, 1, 0.01f); break;
						}

						break;
					}
				}
			}

			ImGui::EndTable();
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

		bool isDynamic = object->IsDynamic;

		if (ImGui::BeginTable("Collision Object", 2)) {
			ImGui::TableSetupColumn("", 0, 0.3f);
			ImGui::TableSetupColumn("", 0, 0.7f);

			PrintCell("Trigger"); PrintEditCell(&object->IsTrigger);
			PrintCell("Static");  PrintEditCell(&object->IsStatic);
			PrintCell("Dynamic"); PrintEditCell(&isDynamic);

			ImGui::EndTable();
		}

		ImGui::PopID();
	}

	void SpaceInspectorSystem::PrintRigidbody(
		iw::Rigidbody* body)
	{
		if (!body) return;

		ImGui::PushID(body);

		if (ImGui::BeginTable("Rigidbody", 2)) {
			ImGui::TableSetupColumn("", 0, 0.3f);
			ImGui::TableSetupColumn("", 0, 0.7f);

			PrintCell("Gravity");          PrintEditCell(&body->Gravity);
			PrintCell("Net Force");        PrintEditCell(&body->NetForce);
			PrintCell("Net Torque");       PrintEditCell(&body->NetTorque);
			PrintCell("Velocity");         PrintEditCell(&body->Velocity);
			PrintCell("Angular Velocity"); PrintEditCell(&body->AngularVelocity);
			PrintCell("Lock");             PrintEditCell(&body->AxisLock);

			PrintCell("Lock axis");

			bool x = body->IsAxisLocked.x;
			bool y = body->IsAxisLocked.y;
			bool z = body->IsAxisLocked.z;

			ImGui::TableNextCell();
			ImGui::Checkbox("X", &x); ImGui::SameLine();
			ImGui::Checkbox("Y", &y); ImGui::SameLine();
			ImGui::Checkbox("Z", &z);

			body->IsAxisLocked = glm::vec3(x, y, z);

			float mass = body->Mass();

			PrintCell("Mass");             PrintEditCell(&mass);
			PrintCell("Takes gravity");    PrintEditCell(&body->TakesGravity);
			PrintCell("Simulate gravity"); PrintEditCell(&body->SimGravity);
			PrintCell("Kinemtaic");        PrintEditCell(&body->IsSimulated);
			PrintCell("Static friction");  PrintEditCell(&body->StaticFriction);
			PrintCell("Dynamic friction"); PrintEditCell(&body->DynamicFriction);
			PrintCell("Restitution");      PrintEditCell(&body->Restitution);

			body->SetMass(mass);

			ImGui::EndTable();
		}

		PrintCollisionObject(body);

		ImGui::PopID();
	}
}

	using namespace Editor;
}
