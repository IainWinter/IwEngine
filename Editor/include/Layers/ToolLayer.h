#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Font.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

namespace iw {
	class ToolLayer
		: public Layer
	{
	private:
		Mesh* plightMesh;
		Mesh* dlightMesh;
		Mesh* cameraMesh;

		Mesh* textMesh;
		iw::ref<Shader> fontShader;
		iw::ref<Font> font;

		Scene* scene;

		Camera* camera;
		Camera* oldcamera;

		EditorCameraControllerSystem* cameraSystem;

	public:
		ToolLayer(
			Scene* scene);

		int Initialize() override;

		void OnPush() override;
		void OnPop() override;

		void PostUpdate() override;

		void ImGui() override;

		bool On(MouseButtonEvent& e) override;
	};
}
