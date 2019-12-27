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
#include "iw/graphics/DirectionalLight.h"

#include "iw/util/io/File.h"

#include "Systems/BulletSystem.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"

#include "iw/graphics/TextureAtlas.h"

GameLayer3D::GameLayer3D()
	: Layer("Game")
{}

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

iw::matrix4 orth;
float b, blend = .5f;

float angle = 1.57f;
float distance = 20;
float fov = 1.17f;
float a, d, f;
float blurAmount = 1;
iw::vector3 p;

iw::matrix4 camBuf[2];

IW::DirectionalLight light;

int GameLayer3D::Initialize()
{
	// Loading shaders and filters

	pbrPipeline    = Asset->Load<IW::Shader>("shaders/pbr.shader");

	light = IW::DirectionalLight(IW::OrthographicCamera(80, 80, -20, 80));
	light.SetLightShader(Asset->Load<IW::Shader>("shaders/shadows/directional.shader"));
	light.SetPostFilter (Asset->Load<IW::Shader>("shaders/filters/gaussian.shader"));
	light.SetNullFilter (Asset->Load<IW::Shader>("shaders/filters/null.shader"));

	IW::TextureAtlas lightsA(2048, 2048, IW::RG,    IW::FLOAT);
	IW::TextureAtlas lightsB(2048, 2048, IW::DEPTH, IW::FLOAT);
	IW::TextureAtlas posts  (2048, 2048, IW::ALPHA, IW::FLOAT);

	IW::RenderTarget shadowTarget { lightsA.GetSubTexture(0), lightsB.GetSubTexture(0) };
	IW::RenderTarget blurTarget { posts.GetSubTexture(0) };



	light.SetShadowTarget(std::make_shared<IW::RenderTarget>(shadowTarget));
	light.SetPostTarget(std::make_shared<IW::RenderTarget>(blurTarget));

	// Create uniform buffer for the camera and set it for the pipelines that need it

	cameraBuffer = Renderer->Device->CreateUniformBuffer(2 * sizeof(iw::matrix4));
	pbrPipeline->Program->SetBuffer("Camera", cameraBuffer);

	// Setup render targets for shadow map and blur filter

	IW::Mesh* floorMesh = IW::MakePlane(10, 10);
	floorMesh->GenTangents();
	floorMesh->Initialize(Renderer->Device);
	
	IW::Mesh* mesh = IW::MakeUvSphere(24, 48);
	mesh->GenTangents();
	mesh->Initialize(Renderer->Device);

	iw::ref<IW::Material> mat = std::make_shared<IW::Material>(pbrPipeline);
	iw::ref<IW::Material> floorMat = std::make_shared<IW::Material>(pbrPipeline);

	mesh->SetMaterial(mat);
	floorMesh->SetMaterial(floorMat);

	iw::ref<IW::Texture> albedo    = Asset->Load<IW::Texture>("textures/metal/albedo.jpg");
	iw::ref<IW::Texture> normal    = Asset->Load<IW::Texture>("textures/metal/normal.jpg");
	iw::ref<IW::Texture> metallic  = Asset->Load<IW::Texture>("textures/metal/metallic.jpg");
	iw::ref<IW::Texture> roughness = Asset->Load<IW::Texture>("textures/metal/roughness.jpg");
	iw::ref<IW::Texture> ao        = Asset->Load<IW::Texture>("textures/metal/ao.jpg");
	albedo   ->Initialize(Renderer->Device);
	normal   ->Initialize(Renderer->Device);
	metallic ->Initialize(Renderer->Device);
	roughness->Initialize(Renderer->Device);
	ao       ->Initialize(Renderer->Device);	

	iw::ref<IW::Texture> talbedo    = Asset->Load<IW::Texture>("textures/tile/albedo.png");
	iw::ref<IW::Texture> tnormal    = Asset->Load<IW::Texture>("textures/tile/normal.png");
	iw::ref<IW::Texture> tmetallic  = Asset->Load<IW::Texture>("textures/tile/metallic.jpg");
	iw::ref<IW::Texture> troughness = Asset->Load<IW::Texture>("textures/tile/roughness.png");
	iw::ref<IW::Texture> tao        = Asset->Load<IW::Texture>("textures/tile/ao.png");
	talbedo   ->Initialize(Renderer->Device);
	tnormal   ->Initialize(Renderer->Device);
	tmetallic ->Initialize(Renderer->Device);
	troughness->Initialize(Renderer->Device);
	tao       ->Initialize(Renderer->Device);

	mesh->Material->SetTexture("albedoMap",    albedo);
	mesh->Material->SetTexture("normalMap",    normal);
	mesh->Material->SetTexture("metallicMap",  metallic);
	mesh->Material->SetTexture("roughnessMap", roughness);
	mesh->Material->SetTexture("aoMap",        ao);
	mesh->Material->SetTexture("shadowMap",    &shadowTarget.Textures[0]);

	floorMesh->Material->SetTexture("albedoMap",    talbedo);
	floorMesh->Material->SetTexture("normalMap",    tnormal);
	floorMesh->Material->SetTexture("metallicMap",  tmetallic);
	floorMesh->Material->SetTexture("roughnessMap", troughness);
	floorMesh->Material->SetTexture("aoMap",        tao);
	floorMesh->Material->SetTexture("shadowMap",    &shadowTarget.Textures[0]);

	lightPositions[0] = iw::vector3( 0, 5, 0);
	lightPositions[1] = iw::vector3( 5, 2, 5);
	lightPositions[2] = iw::vector3( 5, 2, -5);
	lightPositions[3] = iw::vector3(-5, 2, 5);

	lightColors[0] = iw::vector3(1);
	lightColors[1] = iw::vector3(1, 0, 0);
	lightColors[2] = iw::vector3(0, 1, 0);
	lightColors[3] = iw::vector3(0, 0, 1);

	// Entities

	IW::Camera* perspective = new IW::PerspectiveCamera(fov, 1.778f, .01f, 200.0f);
	orth                    = iw::matrix4::create_orthographic(35.56f, 20, -100.0f, 200.0f);

	IW::Entity camera = Space->CreateEntity<IW::Transform, IW::CameraController>();
	IW::Entity player = Space->CreateEntity<IW::Transform, IW::ModelComponent, Player>();
	IW::Entity enemy  = Space->CreateEntity<IW::Transform, IW::ModelComponent, Enemy>();
	IW::Entity floor  = Space->CreateEntity<IW::Transform, IW::ModelComponent>();
	
	Space->SetComponentData<IW::CameraController>(camera, perspective);

	Space->SetComponentData<IW::Transform>     (player, iw::vector3(3, -0.25f, 0), iw::vector3(0.75f));
	Space->SetComponentData<IW::ModelComponent>(player, mesh, 1U);
	Space->SetComponentData<Player>            (player, 4.0f, .18f, .08f);

	Space->SetComponentData<IW::Transform>     (enemy, iw::vector3(0, -0.25f, 0), iw::vector3(0.75f));
	Space->SetComponentData<IW::ModelComponent>(enemy, mesh, 1U);
	Space->SetComponentData<Enemy>             (enemy, SPIN, 0.2617993f, .12f, 0.0f);

	Space->SetComponentData<IW::Transform>     (floor, iw::vector3(0, -1, 0), iw::vector3(15, 1, 15));
	Space->SetComponentData<IW::ModelComponent>(floor, floorMesh, 1U);

	//IW::Entity debug = Space->CreateEntity<IW::Transform, IW::DebugVector>();

	lightDirection = iw::vector3(1, 1, 0);

	PushSystem<EnemySystem>(mesh);
	PushSystem<BulletSystem>();
	PushSystem<PlayerSystem>();

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
	iw::vector3 playerOffset;
	for (auto entity : Space->Query<IW::Transform, Player>()) {
		auto [transform, player] = entity.Components.Tie<PlayerComponents>();

		playerOffset.x = transform->Position.x * .8f;
		playerOffset.z = transform->Position.z * .8f;
	}

	for (auto entity : Space->Query<IW::CameraController>()) {
		auto [controller] = entity.Components.TieTo<IW::CameraController>();

		IW::PerspectiveCamera* cam = (IW::PerspectiveCamera*)controller->Camera;

		bool needsUpdate = false;
		if (a != angle || d != distance || p != playerOffset) {
			a = angle;
			d = distance;
			p = playerOffset;
			cam->Position = iw::vector3(0, sin(angle), cos(angle)) * distance + playerOffset;
			cam->Rotation = iw::quaternion::from_euler_angles(iw::PI + angle, 0, iw::PI);

			camBuf[1] = cam->GetView();
			needsUpdate = true;
		}

		if (f != fov || b != blend) {
			f = fov;
			b = blend;

			iw::matrix4 camA = iw::matrix4::create_perspective_field_of_view(fov, 1.778f, 0.01f, 200.0f); // ew

			iw::matrix4 matrix;
			for (size_t i = 0; i < 16; i++) {
				matrix[i] = camA[i] + (orth[i] - camA[i]) * blend;
			}

			cam->SetProjection(matrix); // ew

			camBuf[0] = cam->GetProjection();
			needsUpdate = true;
		}

		if (needsUpdate) {
			Renderer->Device->UpdateUniformBufferData(cameraBuffer, camBuf);
		}

		// What would be nice
		// Renderer->BeginScene(scenedata -- for uniform buffer)
		// loop
		//	Renderer->DrawMesh(mesh)

		//Renderer->End -- unbind stuff?

		light.SetPosition(lightDirection);
		light.SetRotation(iw::quaternion::from_look_at(lightDirection));

		// Draw shadow texture

		Renderer->BeginLight(&light);

		for (auto tree : Space->Query<IW::Transform, IW::ModelComponent>()) {
			auto [transform, model] = tree.Components.Tie<ModelComponents>();

			for (size_t i = 0; i < model->MeshCount; i++) {
				Renderer->CastMesh(&light, transform, &model->Meshes[i]);
			}
		}

		Renderer->LightPostProcess(&light);

		light.PostProcess();

		Renderer->EndLight(&light);

		// Blur shadow texture

		float blurw = 1.0f / (shadowTarget->Width  * blurAmount);
		float blurh = 1.0f / (shadowTarget->Height * blurAmount);

		blurFilter->Program->GetParam("blurScale")->SetAsFloats(&iw::vector3(blurw, 0, 0), 3);
		Renderer->ApplyFilter(blurFilter->Program, shadowTarget, shadowTargetBlur);

		blurFilter->Program->GetParam("blurScale")->SetAsFloats(&iw::vector3(0, blurh, 0), 3);
		Renderer->ApplyFilter(blurFilter->Program, shadowTargetBlur, shadowTarget);
		
		// Draw actual scene

		Renderer->BeginScene();

		Renderer->Device->SetPipeline(pbrPipeline->Program.get());
		pbrPipeline->Program->GetParam("lightPositions")->SetAsFloats(&lightPositions, 4, 3); // need better way to pass scene data
		pbrPipeline->Program->GetParam("lightColors")->SetAsFloats(&lightColors, 4, 3);
		pbrPipeline->Program->GetParam("camPos")->SetAsFloats(&controller->Camera->Position, 3);
		pbrPipeline->Program->GetParam("lightSpace")->SetAsMat4(lightCam.GetViewProjection());
		//pbrPipeline->GetParam("sunDirection")->SetAsFloats(&lightPositions, 3);

		for (auto tree : Space->Query<IW::Transform, IW::ModelComponent>()) {
			auto [transform, model] = tree.Components.Tie<ModelComponents>();

			for (size_t i = 0; i < model->MeshCount; i++) {
				Renderer->DrawMesh(transform, &model->Meshes[i]);
			}
		}

		Renderer->EndScene();
	}
}

void GameLayer3D::ImGui() {
	ImGui::Begin("Game layer");

	ImGui::Text("Camera");
	ImGui::SliderFloat("Angle", &angle, 0, iw::PI);
	ImGui::SliderFloat("Distance", &distance, 5, 50);
	ImGui::SliderFloat("Fov", &fov, 0.001f, iw::PI - 0.001f);
	ImGui::SliderFloat("Blend", &blend, 0, 1, "%.3f", .5f);

	ImGui::Text("Sun");
	ImGui::SliderFloat3("XYZ", &lightDirection.x, -1, 1);
	ImGui::SliderFloat("Blur", &blurAmount, 0.001f, 1.0f);

	float* s = nullptr;

	for (auto enemy : Space->Query<IW::Transform, Player>()) {
		auto [t, p] = enemy.Components.Tie<PlayerComponents>();

		s = &p->Speed;

		ImGui::Text("Player");
		ImGui::SliderFloat("Speed", &p->Speed, 0.1f, 5.0f);
		ImGui::SliderFloat("Dash Time", &p->DashTime, 0, 1);
		ImGui::SliderFloat("Dash Cooldown", &p->CooldownTime, 0, 1);
		ImGui::Value("Dash Timer", p->Timer);
	}

	for (auto enemy : Space->Query<IW::Transform, Enemy>()) {
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
	//for (auto entity : Space->Query<IW::Transform, IW::CameraController>()) {
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

bool GameLayer3D::On(
	IW::KeyEvent& event)
{
	return Layer::On(event);
}
