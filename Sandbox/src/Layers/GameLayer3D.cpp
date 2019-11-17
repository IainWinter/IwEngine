#include "Layers/GameLayer3D.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"

#include "iw/util/io/File.h"

#include "Systems/BulletSystem.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"

GameLayer3D::GameLayer3D(
	IW::Space& space,
	IW::Renderer& renderer,
	IW::AssetManager& asset)
	: Layer(space, renderer, asset, "Game")
{
	PushSystem<BulletSystem>();
	PushSystem<PlayerSystem>();
}

iw::ref<IW::Shader> pbrPipeline;
iw::ref<IW::Shader> shadowPipeline;
iw::ref<IW::Shader> nullFilter;
iw::ref<IW::Shader> blurFilter;
IW::IFrameBuffer* shadow;
IW::RenderTarget* shadowTarget;
IW::RenderTarget* shadowTargetBlur;

IW::IUniformBuffer* cameraBuffer;

iw::vector3 lightDirection;
iw::matrix4 lightMVP;

float angle = 1.57f;
float distance = 100;
float fov = 0.17f;
float a, d, f;
float blurAmount = 1;

iw::matrix4 camBuf[2];

int GameLayer3D::Initialize(
	IW::InitOptions& options)
{
	// Loading shaders and filters

	pbrPipeline    = Asset.Load<IW::Shader>("shaders/pbr.shader");
	shadowPipeline = Asset.Load<IW::Shader>("shaders/shadows/directional.shader");
	nullFilter     = Asset.Load<IW::Shader>("shaders/filters/null.shader");
	blurFilter     = Asset.Load<IW::Shader>("shaders/filters/gaussian.shader");

	pbrPipeline->Initialize(Renderer.Device);
	shadowPipeline->Initialize(Renderer.Device);
	nullFilter->Initialize(Renderer.Device);
	blurFilter->Initialize(Renderer.Device);

	// Create uniform buffer for the camera and set it for the pipelines that need it

	cameraBuffer = Renderer.Device->CreateUniformBuffer(2 * sizeof(iw::matrix4));
	pbrPipeline->Handle->SetBuffer("Camera", cameraBuffer);

	// Setup render targets for shadow map and blur filter

	shadowTarget     = new IW::RenderTarget(1048, 1048, { IW::RG, IW::DEPTH }, { IW::FLOAT, IW::FLOAT });
	shadowTargetBlur = new IW::RenderTarget(1048, 1048, { IW::ALPHA }, { IW::FLOAT });
	shadowTarget->Initialize(Renderer.Device);
	shadowTargetBlur->Initialize(Renderer.Device);

	iw::ref<IW::Model> floorMesh = Asset.Load<IW::Model>("quad.obj");

	iw::vector2 uvs[4] = {
		iw::vector2(1, 0),
		iw::vector2(1, 1),
		iw::vector2(0, 1),
		iw::vector2(0, 0)
	};

	floorMesh->Meshes->SetUVs(4, uvs);
	floorMesh->Meshes->GenTangents();
	floorMesh->Meshes->Initialize(Renderer.Device);
	
	IW::Mesh* mesh = IW::mesh_factory::create_uvsphere(24, 48);
	mesh->GenTangents();
	mesh->Initialize(Renderer.Device);

	material = std::make_shared<IW::Material>(pbrPipeline->Handle);
	mesh->SetMaterial(material);

	iw::ref<IW::Texture> albedo    = Asset.Load<IW::Texture>("textures/metal/albedo.jpg");
	iw::ref<IW::Texture> normal    = Asset.Load<IW::Texture>("textures/metal/normal.jpg");
	iw::ref<IW::Texture> metallic  = Asset.Load<IW::Texture>("textures/metal/metallic.jpg");
	iw::ref<IW::Texture> roughness = Asset.Load<IW::Texture>("textures/metal/roughness.jpg");
	iw::ref<IW::Texture> ao        = Asset.Load<IW::Texture>("textures/metal/ao.jpg");
	albedo   ->Initialize(Renderer.Device);
	normal   ->Initialize(Renderer.Device);
	metallic ->Initialize(Renderer.Device);
	roughness->Initialize(Renderer.Device);
	ao       ->Initialize(Renderer.Device);

	iw::ref<IW::Texture> talbedo    = Asset.Load<IW::Texture>("textures/tile/albedo.png");
	iw::ref<IW::Texture> tnormal    = Asset.Load<IW::Texture>("textures/tile/normal.png");
	iw::ref<IW::Texture> tmetallic  = Asset.Load<IW::Texture>("textures/tile/metallic.jpg");
	iw::ref<IW::Texture> troughness = Asset.Load<IW::Texture>("textures/tile/roughness.png");
	iw::ref<IW::Texture> tao        = Asset.Load<IW::Texture>("textures/tile/ao.png");
	talbedo   ->Initialize(Renderer.Device);
	tnormal   ->Initialize(Renderer.Device);
	tmetallic ->Initialize(Renderer.Device);
	troughness->Initialize(Renderer.Device);
	tao       ->Initialize(Renderer.Device);

	material->SetTexture("albedoMap",    albedo);
	material->SetTexture("normalMap",    normal);
	material->SetTexture("metallicMap",  metallic);
	material->SetTexture("roughnessMap", roughness);
	material->SetTexture("aoMap",        ao);
	material->SetTexture("shadowMap",    &shadowTarget->Textures[0]);

	floorMesh->Meshes[0].Material->SetTexture("albedoMap",    talbedo);
	floorMesh->Meshes[0].Material->SetTexture("normalMap",    tnormal);
	floorMesh->Meshes[0].Material->SetTexture("metallicMap",  tmetallic);
	floorMesh->Meshes[0].Material->SetTexture("roughnessMap", troughness);
	floorMesh->Meshes[0].Material->SetTexture("aoMap",        tao);
	floorMesh->Meshes[0].Material->SetTexture("shadowMap",    &shadowTarget->Textures[0]);

	floorMesh->Meshes[0].Material->Pipeline = pbrPipeline->Handle;

	lightPositions[0] = iw::vector3( 0, 5, 0);
	lightPositions[1] = iw::vector3( 5, 2, 5);
	lightPositions[2] = iw::vector3( 5, 2, -5);
	lightPositions[3] = iw::vector3(-5, 2, 5);

	lightColors[0] = iw::vector3(1);
	lightColors[1] = iw::vector3(1, 0, 0);
	lightColors[2] = iw::vector3(0, 1, 0);
	lightColors[3] = iw::vector3(0, 0, 1);

	// Entities

	IW::Camera* perspective = new IW::PerspectiveCamera(fov, 1.778f, 100.0f, 120.0f);

	IW::Entity camera = Space.CreateEntity<IW::Transform, IW::CameraController>();
	IW::Entity player = Space.CreateEntity<IW::Transform, IW::ModelComponent, Player>();
	IW::Entity enemy  = Space.CreateEntity<IW::Transform, IW::ModelComponent, Enemy>();
	IW::Entity floor  = Space.CreateEntity<IW::Transform, IW::ModelComponent>();
	
	Space.SetComponentData<IW::CameraController>(camera, perspective);

	Space.SetComponentData<IW::Transform>     (player, iw::vector3(3, -0.25f, 0), iw::vector3(0.75f));
	Space.SetComponentData<IW::ModelComponent>(player, mesh, 1U);
	Space.SetComponentData<Player>            (player, 4.0f, .15f, .05f);

	Space.SetComponentData<IW::Transform>     (enemy, iw::vector3(0, -0.25f, 0), iw::vector3(0.75f));
	Space.SetComponentData<IW::ModelComponent>(enemy, mesh, 1U);
	Space.SetComponentData<Enemy>             (enemy, SPIN, 0.2617993f, .12f, 0.0f);

	Space.SetComponentData<IW::Transform>     (floor, iw::vector3(0, -1, 0), iw::vector3(20, 1, 20), iw::quaternion::from_euler_angles(iw::PI / 2, 0, 0));
	Space.SetComponentData<IW::ModelComponent>(floor, floorMesh->Meshes, 1U);

	//IW::Entity debug = Space.CreateEntity<IW::Transform, IW::DebugVector>();

	lightDirection = iw::vector3(0, 1, 0);

	PushSystem<EnemySystem>(mesh);

	return 0;
}

struct CameraComponents {
	IW::Transform* Transform;
	IW::CameraController* Controller;
};

struct ModelComponents {
	IW::Transform* Transform;
	IW::ModelComponent* Model;
};

struct PlayerComponents {
	IW::Transform* Transform;
	Player* Player;
};

struct EnemyComponents {
	IW::Transform* Transform;
	Enemy* Enemy;
};

void GameLayer3D::PostUpdate() {
	for (auto entity : Space.Query<IW::CameraController>()) {
		auto [controller] = entity.Components.TieTo<IW::CameraController>();

		IW::PerspectiveCamera* cam = (IW::PerspectiveCamera*)controller->Camera;

		bool needsUpdate = false;
		if (a != angle || d != distance) {
			a = angle;
			d = distance;
			cam->Position = iw::vector3(0, sin(angle), cos(angle)) * distance;
			cam->Rotation = iw::quaternion::from_euler_angles(iw::PI + angle, 0, iw::PI);

			camBuf[1] = cam->GetView();
			needsUpdate = true;
		}

		if (f != fov) {
			f = fov;
			cam->SetProjection(fov, 1.778f, 50.0f, 300.0f); // ew

			camBuf[0] = cam->GetProjection();
			needsUpdate = true;
		}

		if (needsUpdate) {
			Renderer.Device->UpdateUniformBufferData(cameraBuffer, camBuf);
		}

		// What would be nice
		// Renderer.BeginScene(scenedata -- for uniform buffer)
		// loop
		//	Renderer.DrawMesh(mesh)

		//Renderer.End -- unbind stuff?



		// Draw shadow texture

		IW::OrthographicCamera lightCam = IW::OrthographicCamera(80, 80, -20, 80);
		lightCam.Position = lightDirection;
		lightCam.Rotation = iw::quaternion::from_look_at(lightDirection);

		Renderer.BeginScene(shadowTarget);

		Renderer.Device->SetPipeline(shadowPipeline->Handle.get());

		shadowPipeline->Handle->GetParam("viewProjection")
			->SetAsMat4(lightCam.GetView() * lightCam.GetProjection());

		for (auto tree : Space.Query<IW::Transform, IW::ModelComponent>()) {
			auto [transform, model] = tree.Components.Tie<ModelComponents>();

			shadowPipeline->Handle->GetParam("model")
				->SetAsMat4(transform->Transformation());

			for (size_t i = 0; i < model->MeshCount; i++) {
				model->Meshes[i].Draw(Renderer.Device);
			}
		}

		Renderer.EndScene();

		// Blur shadow texture

		float blurw = 1.0f / (shadowTarget->Width  * blurAmount);
		float blurh = 1.0f / (shadowTarget->Height * blurAmount);

		blurFilter->Handle->GetParam("blurScale")->SetAsFloats(&iw::vector3(blurw, 0, 0), 3);
		Renderer.ApplyFilter(blurFilter->Handle, shadowTarget, shadowTargetBlur);

		blurFilter->Handle->GetParam("blurScale")->SetAsFloats(&iw::vector3(0, blurh, 0), 3);
		Renderer.ApplyFilter(blurFilter->Handle, shadowTargetBlur, shadowTarget);
		
		// Draw actual scene

		Renderer.BeginScene();

		Renderer.Device->SetPipeline(pbrPipeline->Handle.get());
		pbrPipeline->Handle->GetParam("lightPositions")->SetAsFloats(&lightPositions, 4, 3); // need better way to pass scene data
		pbrPipeline->Handle->GetParam("lightColors")->SetAsFloats(&lightColors, 4, 3);
		pbrPipeline->Handle->GetParam("camPos")->SetAsFloats(&controller->Camera->Position, 3);
		pbrPipeline->Handle->GetParam("lightSpace")->SetAsMat4(lightCam.GetViewProjection());
		//pbrPipeline->GetParam("sunDirection")->SetAsFloats(&lightPositions, 3);

		for (auto tree : Space.Query<IW::Transform, IW::ModelComponent>()) {
			auto [transform, model] = tree.Components.Tie<ModelComponents>();

			for (size_t i = 0; i < model->MeshCount; i++) {
				Renderer.DrawMesh(transform, &model->Meshes[i]);
			}
		}

		Renderer.EndScene();
	}
}

void GameLayer3D::ImGui() {
	ImGui::Begin("Game layer");

	ImGui::Text("Camera");
	ImGui::SliderFloat("Angle", &angle, 0, iw::PI);
	ImGui::SliderFloat("Distance", &distance, 5, 500);
	ImGui::SliderFloat("Fov", &fov, 0.001f, iw::PI / 4);

	ImGui::Text("Sun");
	ImGui::SliderFloat3("XYZ", &lightDirection.x, -1, 1);
	ImGui::SliderFloat("Blur", &blurAmount, 0.001f, 1.0f);

	float* s = nullptr;

	for (auto enemy : Space.Query<IW::Transform, Player>()) {
		auto [t, p] = enemy.Components.Tie<PlayerComponents>();

		s = &p->Speed;

		ImGui::Text("Player");
		ImGui::SliderFloat("Speed", &p->Speed, 0.1f, 5.0f);
		ImGui::SliderFloat("Dash Time", &p->DashTime, 0, 1);
		ImGui::SliderFloat("Dash Cooldown", &p->CooldownTime, 0, 1);
		ImGui::Value("Dash Timer", p->Timer);
	}

	for (auto enemy : Space.Query<IW::Transform, Enemy>()) {
		auto [t, e] = enemy.Components.Tie<EnemyComponents>();

		if (s == &e->Speed) {
			LOG_INFO << "ge";
		}

		ImGui::Value("Enemy", (unsigned int)enemy.Index);
		ImGui::SliderFloat("Enemy Speed", &e->Speed, 0, iw::PI2);
		ImGui::SliderFloat("Fire Time", &e->FireTime, 0, 1);
		ImGui::SliderFloat("Cooldown Time", &e->CooldownTime, 0, 1);
		ImGui::Value("Rotation", e->Rotation);
		ImGui::Value("Fire Timer", e->Timer);
	}

	ImGui::End();
}

bool GameLayer3D::On(
	IW::MouseMovedEvent& event)
{
	//for (auto entity : Space.Query<IW::Transform, IW::CameraController>()) {
	//	auto [transform, controller] = entity.Components.Tie<CameraComponents>();

	//	float pitch = event.DeltaY * 0.0005f;
	//	float yaw   = event.DeltaX * 0.0005f;

	//	iw::quaternion deltaP = iw::quaternion::from_axis_angle(-transform->Right(), pitch);
	//	iw::quaternion deltaY = iw::quaternion::from_axis_angle( iw::vector3::unit_y, yaw);

	//	transform->Rotation *= deltaP * deltaY;

	//	controller->Camera->Rotation = transform->Rotation;
	//}

	return false;
}

