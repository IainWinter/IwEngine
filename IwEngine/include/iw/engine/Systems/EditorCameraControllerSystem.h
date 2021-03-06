#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/engine/Components/EditorCameraController.h"

namespace iw {
namespace Editor {
	class EditorCameraControllerSystem
		: public SystemBase
	{
	private:
		iw::vector3 movement;
		iw::vector2 rotation;
		float speed;
		bool active;

		Camera* camera;
		iw::Transform* cameraTransform;

	public:
		IWENGINE_API
		EditorCameraControllerSystem();

		int Initialize() override;

		void Update() override;
		void OnPush() override;
		void OnPop()  override;

		bool On(KeyEvent& e) override;
		bool On(MouseMovedEvent& e) override;
		bool On(MouseButtonEvent& e) override;
		bool On(MouseWheelEvent& e) override;

		Camera* GetCamera() {
			return camera;
		}
	};
}

	using namespace Editor;
}
