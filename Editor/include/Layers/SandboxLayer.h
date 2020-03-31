#pragma once

#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"
#include "iw/engine/Layer.h"
#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/graphics/DirectionalLight.h"
#include "iw/graphics/Font.h"
#include "iw/util/jobs/pipeline.h"
#include "iw/util/memory/ref.h"

#include "Pipeline/GenerateShadowMap.h"
#include "Pipeline/FilterTarget.h"
#include "Pipeline/Render.h"

namespace iw {
	class SandboxLayer
		: public Layer
	{
	private:
		PlayerSystem* playerSystem;
		EnemySystem*  enemySystem;

		Scene* scene;

		// Lights

		DirectionalLight* sun;
		PointLight* light;

		// Light post process

		//float blurAmount = .5f;

		// Pipeline would be nice needs to redesign for multiple shadow maps

		//iw::pipeline pipeline;
		//GenerateShadowMap* generateShadowMap;
		//FilterTarget* postProcessShadowMap;
		//Render* mainRender;

		// Little bit of text

		Mesh* textMesh;
		Transform textTransform;
		OrthographicCamera* textCam;
		iw::ref<Shader> fontShader;
		iw::ref<Font> font;

		float x = 0;
		int sc = 5;
		std::string str;
		float ts = 1.0f;
		float threshold = 0.25f;

	public:
		SandboxLayer();

		int Initialize() override;

		void PostUpdate() override;
		void FixedUpdate() override;

		void ImGui() override;

		bool On(MouseMovedEvent& e) override;
		//bool On(KeyTypedEvent& e) override;
		bool On(ActionEvent& e) override;
	};
}
