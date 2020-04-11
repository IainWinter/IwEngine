#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/engine/Components/EditorCameraController.h"

namespace iw {
namespace Editor {
	class EditorCameraControllerSystem
		: public System<Transform, EditorCameraController>
	{
	public:
		struct Components {
			Transform* Transform;
			EditorCameraController* Controller;
		};
	private:
		iw::vector3 movement;
		iw::vector2 rotation;
		float speed;

	public:
		IWENGINE_API
		EditorCameraControllerSystem();

		//int Initialize() override;

		IWENGINE_API
		void Update(
			EntityComponentArray& eca) override;

		void OnPush() override;
		void OnPop() override;

		IWENGINE_API
		bool On(KeyEvent& e) override;

		IWENGINE_API
		bool On(MouseMovedEvent& e) override;

		IWENGINE_API
		bool On(MouseButtonEvent& e) override;
	};
}

	using namespace Editor;
}
