#pragma once

#include "iw/engine/System.h"
#include <vector>

namespace iw {
namespace Editor {
	class SpaceInspectorSystem
		: public SystemBase
	{
	private:
		std::vector<EntityHandle> m_entities;
		float m_timer;

		EntityHandle m_selected;

	public:
		SpaceInspectorSystem();

		void Update() override;
		void ImGui()  override;
	private:
		void AddEntityToHierarchy(
			EntityHandle& handle,
			iw::Transform* cache);
	};
}

	using namespace Editor;
}
