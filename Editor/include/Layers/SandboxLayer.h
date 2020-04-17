#pragma once

#include "iw/engine/Layer.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"

#include "iw/graphics/DirectionalLight.h"
#include "iw/graphics/Font.h"

#include "iw/util/memory/ref.h"

namespace iw {
	class SandboxLayer
		: public Layer
	{
	private:
		PlayerSystem* playerSystem;
		EnemySystem*  enemySystem;

		// Lights

		DirectionalLight* sun;
		PointLight* light;

		// Light post process

		//float blurAmount = .5f;

		// Little bit of text

		Mesh textMesh;
		Transform textTransform;
		OrthographicCamera* textCam;
		iw::ref<Shader> fontShader;
		iw::ref<Font> font;

		float x = 0;
		int sc = 5;
		std::string str;
		float threshold = 0.25f;

	public:
		SandboxLayer();

		int Initialize() override;

		void PostUpdate() override;

		void ImGui() override;

		bool On(MouseMovedEvent& e) override;
		//bool On(KeyTypedEvent& e) override;
		bool On(ActionEvent& e) override;
	};
}
