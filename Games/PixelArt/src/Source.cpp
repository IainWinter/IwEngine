#include "iw/engine/EntryPoint.h"

#include "iw/engine/Systems/Render/RenderSystem.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

struct PixelationLayer
	: iw::Layer
{
	iw::Mesh m_screen;

	PixelationLayer() : iw::Layer("Pixelation") {}

	int Initialize() {

		iw::ref<iw::Shader> simpleShader = Renderer->InitShader(
			Asset->Load<iw::Shader>("shaders/simple.shader"), 
			iw::CAMERA
		);

		iw::ref<iw::Shader> screenShader = Renderer->InitShader(
			Asset->Load<iw::Shader>("shaders/texture.shader")
		);

		iw::ref<iw::Material> simpleMat = REF<iw::Material>(simpleShader);
		simpleMat->SetTexture("albedoMap", Asset->Load<iw::Texture>("textures/grass_tuft/baseColor.png")/* iw::Color::From255(255, 240, 20)*/);

		iw::MeshDescription description;
		description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
		description.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));

		iw::Mesh simpleMesh = iw::MakeLine(description, 2)->MakeInstance();
		simpleMesh.SetMaterial(simpleMat);

		iw::Entity cube = Space->CreateEntity<iw::Transform, iw::Mesh>();
		cube.Set<iw::Transform>(glm::vec3(0, -2, 0), glm::vec3(10, 10, 10));
		cube.Set<iw::Mesh>(simpleMesh);

		// Render to low res

		bool lowRes = false;

		unsigned lowResWidth  = lowRes ? 128 : Renderer->Width();
		unsigned lowResHeight = lowRes ? 128 : Renderer->Height();

		iw::ref<iw::RenderTarget> lowResTarget = REF<iw::RenderTarget>();
		lowResTarget->AddTexture(REF<iw::Texture>(lowResWidth, lowResHeight));
		lowResTarget->AddTexture(REF<iw::Texture>(lowResWidth, lowResHeight, iw::TEX_2D, iw::DEPTH));

		lowResTarget->Tex(0)->SetFilter(iw::NEAREST);

		// Final screen render that texture onto the screen quad

		iw::ref<iw::Material> screenMat = REF<iw::Material>(screenShader);
		screenMat->SetTexture("texture", lowResTarget->Tex(0));

		m_screen = Renderer->ScreenQuad();
		m_screen.SetMaterial(screenMat);

		// Rendering and camera

		iw::CameraController* controller = PushSystem<iw::EditorCameraControllerSystem>();
										   PushSystem<iw::RenderSystem>(MainScene, lowResTarget, true);

		int error = iw::Layer::Initialize();
		if (error) return error;

		MainScene->SetMainCamera(controller->GetCamera());

		return 0;
	}

	void PostUpdate() {
		Renderer->BeginScene();

		Renderer->DrawMesh(iw::Transform(), m_screen);

		Renderer->EndScene();
	}
};

struct App
	: iw::Application
{
	App() {
		PushLayer<PixelationLayer>();
	}

	int Initialize(
		iw::InitOptions& options)
	{
		iw::ref<iw::Context> main = Input->CreateContext("main");
		
		main->AddDevice(Input->CreateDevice<iw::RawKeyboard>());
		main->AddDevice(Input->CreateDevice<iw::RawMouse>());
		main->AddDevice(Input->CreateDevice<iw::Mouse>());

		main->MapButton(iw::T, "toolbox");
		main->MapButton(iw::I, "imgui");

		return iw::Application::Initialize(options);
	}
};

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	return new App();
}


iw::Application* GetApplicationForEditor() {
	return new App();
}
