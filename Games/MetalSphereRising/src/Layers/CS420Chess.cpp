#include "Layers/CS420Chess.h"

#include "iw/graphics/MeshFactory.h"
#include "iw/engine/Systems/Render/RenderSystem.h"
#include "iw/engine/Systems/Render/ShadowRenderSystem.h"

CS420Chess::CS420Chess()
	: Layer("CS420 Chess")
{}

int CS420Chess::Initialize() {
	iw::ref<iw::Shader> phong = Asset->Load<iw::Shader>("shaders/phong.shader");
	iw::ref<iw::Shader> dirSd = Asset->Load<iw::Shader>("shaders/lights/directional.shader");
	Renderer->InitShader(phong, iw::CAMERA | iw::SHADOWS | iw::LIGHTS);
	Renderer->InitShader(phong, iw::CAMERA);

	iw::ref<iw::Texture> dirShadowColor = iw::ref<iw::Texture>(new iw::Texture(2024, 2024, iw::TEX_2D, iw::RG,    iw::FLOAT, iw::BORDER));
	iw::ref<iw::Texture> dirShadowDepth = iw::ref<iw::Texture>(new iw::Texture(2024, 2024, iw::TEX_2D, iw::DEPTH, iw::FLOAT, iw::BORDER));
	iw::ref<iw::RenderTarget> dirShadowTarget = REF<iw::RenderTarget>();
	dirShadowTarget = REF<iw::RenderTarget>();
	dirShadowTarget->AddTexture(dirShadowColor);
	dirShadowTarget->AddTexture(dirShadowDepth);
	dirShadowTarget->Initialize(Renderer->Device);

	iw::Camera* camera = new iw::PerspectiveCamera();
	camera->SetPosition(iw::vector3(0, 15, 5));
	camera->SetRotation(iw::quaternion::from_look_at(camera->Position()));
	MainScene->SetMainCamera(camera);

	iw::DirectionalLight* sun = new iw::DirectionalLight();
	sun->SetRotation(iw::quaternion::from_look_at(iw::vector3(1, 2, 1)));
	sun->SetShadowShader(dirSd);
	sun->SetShadowTarget(dirShadowTarget);
	MainScene->AddLight(sun);

	iw::ref<iw::Material> mat = REF<iw::Material>(phong);
	mat->Set("baseColor", iw::Color::From255(180, 20, 20));
	mat->SetTexture("shadowMap", dirShadowTarget->Tex(0));

	iw::MeshDescription default;
	default.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
	default.DescribeBuffer(iw::bName::NORMAL,   iw::MakeLayout<float>(3));

	iw::Transform transform(iw::vector3(0, 1, 0), iw::vector3(0.8f, 1, 0.8f));
	iw::Mesh cubeMesh = iw::MakeCube(default)->MakeInstance();
	cubeMesh.SetMaterial(mat);

	m_peice.Add(Space->RegisterComponent<iw::Transform>(), &transform);
	m_peice.Add(Space->RegisterComponent<iw::Mesh>(),      &cubeMesh);

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			iw::Entity peice = Space->Instantiate(m_peice);
			peice.Find<iw::Transform>()->Position = iw::vector3(7-i*2, 1, 7-j*2);
		}
	}
	
	iw::Entity floor = Space->CreateEntity<iw::Transform, iw::Mesh>();
					      floor.Set<iw::Transform>(0, 9.6f);
	iw::Mesh* floorMesh = floor.Set<iw::Mesh>(iw::MakePlane(default)->MakeInstance());
	floorMesh->SetMaterial(mat->MakeInstance());
	floorMesh->Material()->Set("baseColor", iw::Color::From255(20, 20, 255));

	PushSystem<iw::RenderSystem>(MainScene);
	PushSystem<iw::ShadowRenderSystem>(MainScene);

	return Layer::Initialize();
}


