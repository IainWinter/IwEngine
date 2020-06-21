#include "Systems/SpaceInspectorSystem.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"

#include "..\imgui\src\imgui_demo.cpp"

namespace iw {
namespace Editor {
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

		ImGui::Text("Entity Handle");
		ImGui::Separator();
		ImGui::Text("Id: %i", m_selectedEntity.Index);
		ImGui::Text("Version: %i", m_selectedEntity.Version);
		
		ImGui::Separator();

		ref<Archetype> archetype = Space->GetArchetype(m_selectedEntity);
		for (unsigned i = 0; i < archetype->Count; i++) {
			ref<Component> component = archetype->Layout[i].Component;

			if (component == Space->GetComponent<iw::Transform>()) {
				iw::Transform*       t = Space->FindComponent<iw::Transform>(m_selectedEntity);
				iw::CollisionObject* c = Space->FindComponent<iw::CollisionObject>(m_selectedEntity);
				iw::Rigidbody*       r = Space->FindComponent<iw::Rigidbody>(m_selectedEntity);

				     if (c) t = &c->Trans();
				else if (r) t = &r->Trans();

				PrintTransform(t);
			}

			else if (component == Space->GetComponent<iw::Mesh>()) {
				PrintMesh(Space->FindComponent<iw::Mesh>(m_selectedEntity));
			}

			else if (component == Space->GetComponent<iw::Model>()) {
				PrintModel(Space->FindComponent<iw::Model>(m_selectedEntity));
			}

			else if (component == Space->GetComponent<iw::ParticleSystem<>>()) {
				PrintParticleSystem(Space->FindComponent<iw::ParticleSystem<>>(m_selectedEntity));
			}

			else {
				ImGui::Text(component->Name);
			}

			ImGui::Separator();
		}

		ImGui::End();

		ImGui::Begin("Resource Inspector");

		if (m_selectedMeshData) {
			ImGui::Text("Name: ");
			ImGui::SameLine();
			ImGui::Text(m_selectedMeshData->Name().c_str());
		
			if (ImGui::TreeNode("Buffers")) {
				for (bName name : m_selectedMeshData->Description().GetBufferNames()) {
					ImGui::Separator();

					ImGui::Text("Name: ");
					ImGui::SameLine();
					ImGui::Text(getbNameStr(name));

					unsigned count = m_selectedMeshData->GetCount(name);

					ImGui::Text("Count: %i", count);

					ImGui::Text("Stride: %i",          m_selectedMeshData->Description().GetBufferLayout(name).GetStride());
					ImGui::Text("Instance Stride: %i", m_selectedMeshData->Description().GetBufferLayout(name).GetInstanceStride());

					if (ImGui::TreeNode((void*)name, "Elements")) {
						auto elements = m_selectedMeshData->Description().GetBufferLayout(name).GetElements();
						for (unsigned j = 0; j < elements.size(); j++) {
							ImGui::Text("Type: ");
							ImGui::SameLine();

							switch (elements[j].BType) {
							case BufferLayoutType::FLOAT: {
								ImGui::Text("float");
								break;
							}
							case BufferLayoutType::UINT: {
								ImGui::Text("unsigned int");
								break;
							}
							case BufferLayoutType::UCHAR: {
								ImGui::Text("unsigned char");
								break;
							}
							default: {
								ImGui::Text("unknow");
								break;
							}
							}
						}

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
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
		const iw::Transform* transform)
	{
		ImGui::PushID(transform);

		ImGui::Text("Transform");

		ImGui::DragFloat3("Position", (float*)&transform->Position);
		ImGui::DragFloat3("Scale",    (float*)&transform->Scale);
		ImGui::DragFloat4("Rotation", (float*)&transform->Rotation);

		EntityHandle parent = Space->FindEntity(transform->Parent()).Handle;
		unsigned id = parent.Index;

		if (parent == EntityHandle::Empty) {
			ImGui::Text("Parent: None");
		}

		else {
			bool selected = m_selectedEntity == parent;
			ImGui::Text("Parent: ");
			ImGui::SameLine();
			ImGui::Selectable(PrintName(parent).c_str(), &selected);

			if (selected) {
				m_selectedEntity = parent;
			}
		}

		ImGui::PopID();
	}

	void SpaceInspectorSystem::PrintMesh(
		const iw::Mesh* mesh)
	{
		ImGui::PushID(mesh);

		ImGui::Text("Mesh");

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
		const iw::Model* model)
	{
		ImGui::PushID(model);

		ImGui::Text("Model");

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
		const iw::ParticleSystem<>* system)
	{
		ImGui::PushID(system);

		ImGui::Text("Particle System");

		PrintMesh(&system->GetParticleMesh());
		
		if (ImGui::TreeNode("Particles")) {
			for (const Particle<StaticParticle>& particle : system->Particles()) {
				ImGui::Separator();
				PrintTransform(&particle.Transform);
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}

	using namespace Editor;
}
