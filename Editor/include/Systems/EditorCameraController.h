#pragma once

#include "iw/engine/System.h"
#include "iw/engine/Components/CameraController.h"

namespace iw {
namespace Editor {
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

		//int Initialize() override;

		void Update(
			EntityComponentArray& eca) override;

		bool On(ActionEvent& e) override;
		bool On(MouseMovedEvent& e) override;
		bool On(MouseButtonEvent& e) override;
	};
}

	using namespace Editor;
}
