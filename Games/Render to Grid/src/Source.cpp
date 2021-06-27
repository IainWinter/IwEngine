#include "iw/engine/EntryPoint.h"
#include "iw/graphics/TextureAtlas.h"

struct Layer : iw::Layer {
	
	iw::Mesh meshAtlas;
	iw::Mesh meshScreen;
	iw::ref<iw::TextureAtlas> atlas;
	iw::ref<iw::RenderTarget> target;

	Layer() : iw::Layer("render to grid") {}

	int Initialize() override {
		meshAtlas = Renderer->ScreenQuad().MakeInstance();
		meshAtlas.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/texture2d.shader"));
		meshAtlas.Material->Set("color", iw::Color::From255(100, 100, 100));

		atlas = REF<iw::TextureAtlas>(100 * 100, 100 * 100);
		atlas->GenTexBounds(100, 100);
		atlas->CreateColors();

		target = REF<iw::RenderTarget>();
		target->AddTexture(atlas->GetSubTexture(0));
		

		meshScreen = meshAtlas.MakeInstance();
		meshScreen.Material->SetTexture("texture", atlas);

		return iw::Layer::Initialize();
	}

	void PostUpdate() override {
		Renderer->BeginScene((iw::Camera*)nullptr, target);
		Renderer->	DrawMesh(iw::Transform(), meshAtlas);

		Renderer->AfterDraw([&]() {
			target->ReadPixels(Renderer->Device);
		});

		Renderer->EndScene();

		Renderer->BeginScene();
		Renderer->DrawMesh(iw::Transform(), meshScreen);
		Renderer->EndScene();
	}
};

struct App : iw::Application {
	App() {
		PushLayer<Layer>();
	}
};

iw::Application* CreateApplication(
	iw::InitOptions& options) 
{
	return new App();
}
