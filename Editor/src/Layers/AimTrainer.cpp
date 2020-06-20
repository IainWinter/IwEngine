#include "Layers/AimTrainer.h"

#include "Systems/CarControllerSystem.h"
#include "Systems/GameCameraController.h"
#include "Systems/CarControllerSystem.h"
#include "iw/engine/Systems/LerpCameraControllerSystem.h"
#include "iw/engine/Systems/Render/MeshRenderSystem.h"
#include "iw/engine/Systems/Render/ModelRenderSystem.h"
#include "iw/engine/Systems/Render/MeshShadowRenderSystem.h"
#include "iw/engine/Systems/Render/ModelShadowRenderSystem.h"
#include "iw/engine/Systems/PhysicsSystem.h"

#include "iw/engine/Time.h"

#include "iw/graphics/MeshFactory.h"
#include "iw/physics/Collision/CapsuleCollider.h"
#include "..\..\include\Layers\AimTrainer.h"

#include "imgui/imgui.h"

AimTrainerLayer::AimTrainerLayer()
	: Layer("Aim Training Marching")
{
	srand(time(nullptr));
}

struct MeshComponents {
	iw::Transform* Transform;
	iw::Mesh* Mesh;
};

iw::vector3 boxpos;

int AimTrainerLayer::Initialize() {
	// Shaders

	shader = Asset->Load<iw::Shader>("shaders/phong.shader");
	Renderer->InitShader(shader, iw::CAMERA | iw::SHADOWS | iw::LIGHTS);

	iw::ref<iw::Shader> dirShadowShader = Asset->Load<iw::Shader>("shaders/lights/directional.shader");
	Renderer->InitShader(dirShadowShader, iw::CAMERA);

	// Directional light shadow map textures & target

	iw::ref<iw::Texture> dirShadowColor = iw::ref<iw::Texture>(new iw::Texture(1024, 1024, iw::TEX_2D, iw::RG,    iw::FLOAT));
	iw::ref<iw::Texture> dirShadowDepth = iw::ref<iw::Texture>(new iw::Texture(1024, 1024, iw::TEX_2D, iw::DEPTH, iw::FLOAT));

	iw::ref<iw::RenderTarget> dirShadowTarget = REF<iw::RenderTarget>();
	dirShadowTarget->AddTexture(dirShadowColor);
	dirShadowTarget->AddTexture(dirShadowDepth);

	dirShadowTarget->Initialize(Renderer->Device);

	//	Lights

	iw::DirectionalLight* sun = new iw::DirectionalLight(100, iw::OrthographicCamera(60, 32, -100, 100), dirShadowTarget, dirShadowShader);
	sun->SetRotation(iw::quaternion(0.872f, 0.0f, 0.303f, 0.384f));

	MainScene->AddLight(sun);
	MainScene->SetMainCamera(new iw::PerspectiveCamera(1.17f, 1.778f, 0.1f, 500.0f));

	iw::Entity camera = Space->CreateEntity<iw::Transform, iw::CameraController>();

	iw::Transform* t = camera.Set<iw::Transform>(iw::vector3(0, 5, 0));
	                   camera.Set<iw::CameraController>(MainScene->MainCamera());

	MainScene->MainCamera()->SetTrans(t);

	iw::MeshDescription description;
	description.DescribeBuffer(iw::bName::POSITION,  iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::NORMAL,    iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::TANGENT,   iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::BITANGENT, iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::UV,        iw::MakeLayout<float>(2));

	iw::Mesh box = iw::MakeCube(description)->MakeInstance();// Asset->Load<iw::Model>("models/block/forest02.dae")->GetMesh(1).MakeInstance();// iw::MakeCube(description)->MakeInstance();

	//box.Material()->SetShader(shader);

	iw::Material mat(shader);
	mat.Set("baseColor", iw::vector4(1, 1, 1, 1));
	//mat.Set("ao", 0);
	//mat.Set("reflectance", 0.2f);
	mat.SetTexture("shadowMap", dirShadowTarget->Tex(0));    // shouldnt really be part of material
	mat.Initialize(Renderer->Device);

	box.SetMaterial(Asset->Give<iw::Material>("materials/default", &mat));

	iw::Entity player = Space->CreateEntity<iw::Transform, iw::Mesh>();

	                    player.Set<iw::Mesh>(box);
	iw::Transform* tt = player.Set<iw::Transform>(iw::vector3(0, 5, -5));

	PushSystem<iw::MeshShadowRenderSystem>(MainScene);
	PushSystem<iw::ModelShadowRenderSystem>(MainScene);
	//PushSystem<iw::ParticleShadowRenderSystem>(MainScene);
	PushSystem<iw::MeshRenderSystem>(MainScene);
	PushSystem<iw::ModelRenderSystem>(MainScene);
	//PushSystem<iw::ParticleRenderSystem>(MainScene);
	//PushSystem<iw::LerpCameraControllerSystem>(player);
	//PushSystem<iw::PhysicsSystem>();

	return Layer::Initialize();
}

void AimTrainerLayer::PostUpdate() {
	for (auto e : Space->Query<iw::Transform, iw::Mesh>()) {
		auto [t, m] = e.Components.Tie<MeshComponents>();
		t->Position = boxpos;
	}
}

void AimTrainerLayer::ImGui() {
	ImGui::Begin("Aim Trainer");

	ImGui::SliderFloat3("Box", (float*)&boxpos, -10, 10);

	ImGui::End();
}
