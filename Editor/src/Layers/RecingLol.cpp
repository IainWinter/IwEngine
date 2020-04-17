#include "Layers/RacingLol.h"
#include "Systems/CarControllerSystem.h"
#include "Systems/GameCameraController.h"

#include "Systems/CarControllerSystem.h"
#include "iw/engine/Systems/LerpCameraControllerSystem.h"
#include "iw/engine/Systems/MeshRenderSystem.h"
#include "iw/engine/Systems/ModelRenderSystem.h"
#include "iw/engine/Systems/LightRenderSystem.h"

#include "iw/graphics/MeshFactory.h"
#include "iw/engine/Time.h"
#include "iw/physics/Collision/CapsuleCollider.h"
#include "iw/engine/Systems/PhysicsSystem.h"

RecingLayer::RecingLayer()
	: Layer("Ray Marching")
{
	srand(time(nullptr));
}

int RecingLayer::Initialize() {
	// Shaders

	shader = Asset->Load<iw::Shader>("shaders/pbr.shader");
	Renderer->InitShader(shader, iw::CAMERA | iw::SHADOWS | iw::LIGHTS);

	iw::ref<iw::Shader> dirShadowShader = Asset->Load<iw::Shader>("shaders/lights/directional.shader");
	Renderer->InitShader(dirShadowShader, iw::CAMERA);

	// Directional light shadow map textures & target

	iw::ref<iw::Texture> dirShadowColor = iw::ref<iw::Texture>(new iw::Texture(1024, 1024, iw::TEX_2D, iw::RG,    iw::FLOAT, iw::BORDER));
	iw::ref<iw::Texture> dirShadowDepth = iw::ref<iw::Texture>(new iw::Texture(1024, 1024, iw::TEX_2D, iw::DEPTH, iw::FLOAT, iw::BORDER));

	iw::ref<iw::RenderTarget> dirShadowTarget = REF<iw::RenderTarget>(1024, 1024);
	dirShadowTarget->AddTexture(dirShadowColor);
	dirShadowTarget->AddTexture(dirShadowDepth);

	dirShadowTarget->Initialize(Renderer->Device);

	//	Lights

	iw::DirectionalLight* sun = new iw::DirectionalLight(100, iw::OrthographicCamera(60, 32, -100, 100), dirShadowShader, dirShadowTarget);
	sun->SetRotation(iw::quaternion(0.872f, 0.0f, 0.303f, 0.384f));

	MainScene->AddLight(sun);
	MainScene->SetMainCamera(new iw::PerspectiveCamera(1.17f, 1.778f, 1.0f, 500.0f));

	iw::ref<iw::Model> model = Asset->Load<iw::Model>("models/block/forest04.dae");
	for (iw::Mesh& mesh : *model) {
		mesh.Material()->SetTexture("shadowMap", dirShadowColor); // shouldnt be part of material
		mesh.Material()->Set("roughness", 0.9f);
		mesh.Material()->Set("metallic", 0.1f);
		mesh.Material()->SetShader(shader);
	}

	for (int i = 0; i < 50; i++) {
		iw::Entity entity = Space->CreateEntity<iw::Transform, iw::Model>();

		entity.SetComponent<iw::Transform>(iw::vector3(64 * i, 0, 0));
		entity.SetComponent<iw::Model>(*model);
	}

	iw::Entity camera = Space->CreateEntity<iw::Transform, iw::CameraController>();

	iw::Transform* t = camera.SetComponent<iw::Transform>(iw::vector3(0, 5, 0));
	camera.SetComponent<iw::CameraController>(MainScene->MainCamera());

	MainScene->MainCamera()->SetTrans(t);

	iw::ref<iw::Material> mat = model->GetMesh(0).Material()->MakeInstance();
	mat->Set("albedo", iw::Color::From255(1, 1, 1, 1));
	mat->Set("roughness", 0.2f);
	mat->Set("metallic", 1.0f);

	iw::Mesh box = iw::MakeCapsule(model->GetMesh(0).Data()->Description(), 15)->MakeInstance();
	box.SetMaterial(mat);

	iw::Entity player = Space->CreateEntity<iw::Transform, iw::Mesh, iw::CapsuleCollider, iw::Rigidbody, Car>();

	                          player.SetComponent<iw::Mesh>(box);
	iw::Transform* tt =       player.SetComponent<iw::Transform>(iw::vector3(0, 5, 0), 1, iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::Pi / 2));
	iw::Rigidbody* r =        player.SetComponent<iw::Rigidbody>();
	iw::CapsuleCollider* c  = player.SetComponent<iw::CapsuleCollider>();

	PushSystem<iw::MeshRenderSystem>(MainScene);
	PushSystem<iw::ModelRenderSystem>(MainScene);
	PushSystem<iw::LightRenderSystem>(MainScene);
	PushSystem<iw::LerpCameraControllerSystem>(player);
	PushSystem<CarControllerSystem>();
	PushSystem<iw::PhysicsSystem>();

	r->SetTrans(tt);
	r->SetCol(c);


	Physics->AddRigidbody(r);

	return Layer::Initialize();
}

void RecingLayer::PostUpdate() {

}

void RecingLayer::FixedUpdate() {
	Physics->Step(iw::Time::FixedTime());
}

void RecingLayer::ImGui() {
	//ImGui::Begin("Test");

	//ImGui::End();
}

