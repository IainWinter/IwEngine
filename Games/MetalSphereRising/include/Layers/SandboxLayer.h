#pragma once

#include "iw/engine/Layer.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"
#include "Systems/EnemyBossSystem.h"
#include "Systems/BulletSystem.h"

#include "iw/graphics/DirectionalLight.h"
#include "iw/graphics/Font.h"

#include "iw/util/memory/ref.h"

namespace iw {
	class SandboxLayer
		: public Layer
	{
	private:
		PlayerSystem* playerSystem;
		EnemySystem* enemySystem;
		EnemyBossSystem* enemyBossSystem;
		BulletSystem* bulletSystem;

		// Shaders

		ref<Shader> vct;

		// Lights

		DirectionalLight* sun;
		PointLight* light;

		// Light post process

		//float blurAmount = .5f;

		// Little bit of text

		OrthographicCamera* m_textCam;
		Mesh* m_textMesh;
		ref<Font> m_font;

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
