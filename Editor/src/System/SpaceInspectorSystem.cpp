#include "Systems/SpaceInspectorSystem.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"

#include "..\imgui\src\imgui_demo.cpp"

#include "iw/common/Components/Transform.h"

namespace iw {
namespace Editor {
	SpaceInspectorSystem::SpaceInspectorSystem()
		: SystemBase("Space Inspector")
		, m_timer(0.0f)
		, m_selected()
	{}

	void SpaceInspectorSystem::Update() {
		m_timer += iw::Time::DeltaTime();

		if (m_timer > 1.0f / 60) {
			m_timer = 0;

			m_entities.swap(Space->Entities());
		}
	}

	void SpaceInspectorSystem::ImGui() {
		ImGui::Begin("Space Inspector");

		for (EntityHandle handle : m_entities) {
			AddEntityToHierarchy(handle, nullptr);
		}

		ImGui::End();

		ImGui::Begin("Entity Inspector");

		if (m_selected == EntityHandle::Empty) return;

		ref<Archetype> archetype = Space->GetArchetype(m_selected);
		for (unsigned i = 0; i < archetype->Count; i++) {
			ref<Component> component = archetype->Layout[i].Component;
			ImGui::Text(component->Name);

			if (component == Space->GetComponent<iw::Transform>()) {
				iw::Transform* t = Space->FindComponent<iw::Transform>(m_selected);

				ImGui::DragFloat3("Position: ", (float*)&t->Position);
				ImGui::DragFloat3("Scale: ",    (float*)&t->Scale);
				ImGui::DragFloat4("Rotation: ", (float*)&t->Rotation);
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

		std::stringstream ss;
		ss << "Entity id: " << handle.Index << " v: " << handle.Version;

		// No transform or a transform that is in the root & has no children
		if (    !transform
			|| ((cache || !transform->Parent()) && transform->ChildCount() == 0))
		{
			bool selected = m_selected == handle;
			ImGui::Selectable(ss.str().c_str(), &selected);

			if (selected) {
				m_selected = handle;
			}

			return;
		}

		// If the transform has a parent skip it because it'l get hit when the parent is iterated
		if (!cache && transform->Parent()) return;

		if (ImGui::TreeNode(ss.str().c_str())) {
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
}

	using namespace Editor;
}
