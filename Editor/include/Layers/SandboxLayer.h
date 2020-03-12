#pragma once 

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
		PerspectiveCamera* mainCam;

		OrthographicCamera* tranCam;
		iw::Mesh* tranMesh;

		DirectionalLight light;
		iw::vector3 lightPos = iw::vector3(-1.25, 5, -1.75f);
		iw::ref<RenderTarget> target;
		iw::ref<RenderTarget> targetBlur;
		iw::ref<Shader> gaussian;
		float blurAmount = .5f;

		iw::pipeline pipeline;
		GenerateShadowMap* generateShadowMap;
		FilterTarget* postProcessShadowMap;
		Render* mainRender;

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

		void PreUpdate() override;
		void PostUpdate() override;
		void FixedUpdate() override;

		void ImGui() override;

		bool On(MouseMovedEvent& e) override;
		bool On(KeyTypedEvent& e) override;
		bool On(ActionEvent& e) override;
	};
}
