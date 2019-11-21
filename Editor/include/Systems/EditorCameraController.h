#pragma once

#include "iw/engine/System.h"
#include "iw/engine/Components/CameraController.h"

namespace IW {
inline namespace Editor {
	class EditorCameraController
		: public System<Transform, CameraController>
	{
	public:
		struct Components {
			Transform*        Transform;
			CameraController* Controller;
		};
	private:
		iw::vector3 movement;
		iw::vector2 rotation;
		float speed;

	public:
		EditorCameraController();

		void Update(
			EntityComponentArray& eca);

		bool On(KeyEvent& e);
		bool On(MouseMovedEvent& e);
		bool On(MouseButtonEvent& e);
	};
}
}
