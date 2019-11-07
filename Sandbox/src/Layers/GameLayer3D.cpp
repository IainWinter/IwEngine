#include "Layers/GameLayer3D.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"
#include "iw/graphics/MeshFactory.h"

#include "iw/util/io/File.h"

#include "Systems/BulletSystem.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"

GameLayer3D::GameLayer3D(
	IwEntity::Space& space, 
	IW::Renderer& renderer,
	IW::AssetManager& asset)
	: Layer(space, renderer, asset, "Game")
{
	PushSystem<BulletSystem>();
	PushSystem<PlayerSystem>();
}

iwu::ref<IW::IPipeline> pbr;
iwu::ref<IW::IPipeline> pipeline;
iwu::ref<IW::IPipeline> shadowPipeline;
iwu::ref<IW::IPipeline> nullFilter;
iwu::ref<IW::IPipeline> gausFilter;
IW::IFrameBuffer* shadow;
IW::RenderTarget* shadowTarget;
IW::RenderTarget* shadowTargetBlur;

iwm::vector3 lightDirection;
iwm::matrix4 lightMVP;

float angle = 1.396263f;
float distance = 100;
float fov = 0.17f;
float a, d, f;
float blurAmount = 1;

int GameLayer3D::Initialize(
	IwEngine::InitOptions& options)
{
	pbr = Renderer.CreatePipeline("assets/shaders/pbr/pbrvs.glsl", "assets/shaders/pbr/pbrfs.glsl");
	pipeline = Renderer.CreatePipeline("assets/shaders/sandboxvs.glsl", "assets/shaders/sandboxfs.glsl");
	shadowPipeline = Renderer.CreatePipeline("assets/shaders/shadows/directionalvs.glsl", "assets/shaders/shadows/directionalfs.glsl");
	nullFilter = Renderer.CreatePipeline("assets/shaders/filters/nonevs.glsl", "assets/shaders/filters/nonefs.glsl");
	gausFilter = Renderer.CreatePipeline("assets/shaders/filters/gaussianvs.glsl", "assets/shaders/filters/gaussianfs.glsl");

	iwm::vector2 uvs[4] = {
		iwm::vector2(1, 0),
		iwm::vector2(1, 1),
		iwm::vector2(0, 1),
		iwm::vector2(0, 0)
	};

	shadowTarget     = new IW::RenderTarget(4096, 4096, { IW::RG, IW::DEPTH }, { IW::FLOAT, IW::FLOAT });
	shadowTargetBlur = new IW::RenderTarget(4096, 4096, { IW::RG }, { IW::FLOAT });
	shadowTarget->Initialize(Renderer.Device);
	shadowTargetBlur->Initialize(Renderer.Device);

	//shadow = Renderer.Device->CreateFrameBuffer();

	//shadowTexture = std::make_shared<IW::Texture>(1024, 1024, IW::RG, IW::FLOAT, nullptr);
	//shadowTextureDepth = std::make_shared<IW::Texture>(1024, 1024, IW::DEPTH, IW::FLOAT, nullptr);
	//shadowTexture->Initialize(Renderer.Device);
	//shadowTextureDepth->Initialize(Renderer.Device);

	//Renderer.Device->AttachTextureToFrameBuffer(shadow, shadowTexture->Handle);
	//Renderer.Device->AttachTextureToFrameBuffer(shadow, shadowTextureDepth->Handle);

	iwu::ref<IW::Model> floorMesh = Asset.Load<IW::Model>("quad.obj");
	floorMesh->Meshes->SetUVs(4, uvs);
	floorMesh->Meshes->GenTangents();
	floorMesh->Meshes->Initialize(Renderer.Device);
	
	IW::Mesh* mesh = IW::mesh_factory::create_uvsphere(24, 48);
	mesh->GenTangents();
	mesh->Initialize(Renderer.Device);

	material = std::make_shared<IW::Material>(pbr);
	mesh->SetMaterial(material);

	iwu::ref<IW::Texture> albedo = Asset.Load<IW::Texture>("textures/metal/albedo.jpg");
	iwu::ref<IW::Texture> normal = Asset.Load<IW::Texture>("textures/metal/normal.jpg");
	iwu::ref<IW::Texture> metallic = Asset.Load<IW::Texture>("textures/metal/metallic.jpg");
	iwu::ref<IW::Texture> roughness = Asset.Load<IW::Texture>("textures/metal/roughness.jpg");
	iwu::ref<IW::Texture> ao = Asset.Load<IW::Texture>("textures/metal/ao.jpg");
	albedo->Initialize(Renderer.Device);
	normal->Initialize(Renderer.Device);
	metallic->Initialize(Renderer.Device);
	roughness->Initialize(Renderer.Device);
	ao->Initialize(Renderer.Device);

	iwu::ref<IW::Texture> talbedo = Asset.Load<IW::Texture>("textures/tile/albedo.png");
	iwu::ref<IW::Texture> tnormal = Asset.Load<IW::Texture>("textures/tile/normal.png");
	iwu::ref<IW::Texture> tmetallic = Asset.Load<IW::Texture>("textures/tile/metallic.jpg");
	iwu::ref<IW::Texture> troughness = Asset.Load<IW::Texture>("textures/tile/roughness.png");
	iwu::ref<IW::Texture> tao = Asset.Load<IW::Texture>("textures/tile/ao.png");
	talbedo->Initialize(Renderer.Device);
	tnormal->Initialize(Renderer.Device);
	tmetallic->Initialize(Renderer.Device);
	troughness->Initialize(Renderer.Device);
	tao->Initialize(Renderer.Device);

	material->SetTexture("albedoMap", albedo);
	material->SetTexture("normalMap", normal);
	material->SetTexture("metallicMap", metallic);
	material->SetTexture("roughnessMap", roughness);
	material->SetTexture("aoMap", ao);
	material->SetTexture("shadowMap", &shadowTarget->Textures[0]);

	floorMesh->Meshes[0].Material->SetTexture("albedoMap", talbedo);
	floorMesh->Meshes[0].Material->SetTexture("normalMap", tnormal);
	floorMesh->Meshes[0].Material->SetTexture("metallicMap", tmetallic);
	floorMesh->Meshes[0].Material->SetTexture("roughnessMap", troughness);
	floorMesh->Meshes[0].Material->SetTexture("aoMap", tao);
	floorMesh->Meshes[0].Material->SetTexture("shadowMap", &shadowTarget->Textures[0]);


	//material->SetFloats("albedo", &iwm::vector3(1.0f, 0.85f, 0.57f), 3);
	//material->SetFloat ("metallic", 1.0f);
	//material->SetFloat ("roughness", 0.6f);
	//material->SetFloat ("ao", 1.0f);
	material->Pipeline = pbr;
	floorMesh->Meshes[0].Material->Pipeline = pbr;

	lightPositions[0] = iwm::vector3( 0, 5, 0);
	lightPositions[1] = iwm::vector3( 5, 2, 5);
	lightPositions[2] = iwm::vector3( 5, 2, -5);
	lightPositions[3] = iwm::vector3(-5, 2, 5);

	lightColors[0] = iwm::vector3(1);
	lightColors[1] = iwm::vector3(1, 0, 0);
	lightColors[2] = iwm::vector3(0, 1, 0);
	lightColors[3] = iwm::vector3(0, 0, 1);

	IW::Camera* perspective = new IW::PerspectiveCamera(fov, 1.778f, 0.001f, 1000.0f);

	IwEntity::Entity camera = Space.CreateEntity<IW::Transform, IwEngine::CameraController>();
	Space.SetComponentData<IwEngine::CameraController>(camera, perspective);

	IwEntity::Entity player = Space.CreateEntity<IW::Transform, IwEngine::Model, Player>();
	Space.SetComponentData<IW::Transform>(player, iwm::vector3(3, 0, 0));
	Space.SetComponentData<IwEngine::Model>(player, mesh, 1U);
	Space.SetComponentData<Player>(player, 10.0f, 100.0f, 0.1666f, 0.1f);

	IwEntity::Entity enemy = Space.CreateEntity<IW::Transform, IwEngine::Model, Enemy>();
	Space.SetComponentData<IW::Transform>  (enemy, iwm::vector3(0, 0, 0));
	Space.SetComponentData<IwEngine::Model>(enemy, mesh, 1U);
	Space.SetComponentData<Enemy>          (enemy, SPIN, 3.0f, 0.05f, 0.025f, 0.025f);

	IwEntity::Entity floor = Space.CreateEntity<IW::Transform, IwEngine::Model>();
	Space.SetComponentData<IW::Transform>  (floor, iwm::vector3(0, -1, 0), iwm::vector3(10, 1, 10), 
		iwm::quaternion::from_euler_angles(iwm::PI / 2, 0, 0));
	Space.SetComponentData<IwEngine::Model>(floor, floorMesh->Meshes, 1U);

	IwEntity::Entity f = Space.CreateEntity<IW::Transform, IwEngine::Model>();
	Space.SetComponentData<IW::Transform>(f, iwm::vector3(5, 2, 2), iwm::vector3::one, iwm::quaternion::from_euler_angles(iwm::PI / 2, 0, 0));
	Space.SetComponentData<IwEngine::Model>(f, floorMesh->Meshes, 1U);

	lightDirection = iwm::vector3(1);

	PushSystem<EnemySystem>(mesh);

	return 0;
}

struct PlayerComponents {
	IW::Transform* Transform;
	IwEngine::CameraController* Controller;
};

struct TreeComponents {
	IW::Transform* Transform;
	IwEngine::Model* Model;
};

void GameLayer3D::PostUpdate() {
	for (auto entity : Space.Query<IwEngine::CameraController>()) {
		auto [controller] = entity.Components.TieTo<IwEngine::CameraController>();

		IW::PerspectiveCamera* cam = (IW::PerspectiveCamera*)controller->Camera;

		if (a != angle || d != distance) {
			a = angle;
			d = distance;
			cam->Position = iwm::vector3(0, sin(angle), cos(angle)) * distance;
			cam->Rotation = iwm::quaternion::from_euler_angles(iwm::PI + angle, 0, iwm::PI);
		}

		if (f != fov) {
			f = fov;
			cam->SetProjection(fov, 1.778f, 0.001f, 1000.0f); // ew

		}

		IW::OrthographicCamera lightCam = IW::OrthographicCamera(30, 30, -20, 20);
		lightCam.Position = lightDirection;
		lightCam.Rotation = iwm::quaternion::from_look_at(lightDirection);

		Renderer.BeginScene(&lightCam, shadowTarget);

		Renderer.Device->SetPipeline(shadowPipeline.get());

		for (auto tree : Space.Query<IW::Transform, IwEngine::Model>()) {
			auto [transform, model] = tree.Components.Tie<TreeComponents>();

			shadowPipeline->GetParam("model")
				->SetAsMat4(transform->Transformation());

			for (size_t i = 0; i < model->MeshCount; i++) {
				model->Meshes[i].Draw(Renderer.Device);
			}
		}

		Renderer.EndScene();

		float blurw = 1.0f / (shadowTarget->Width  * blurAmount);
		float blurh = 1.0f / (shadowTarget->Height * blurAmount);

		gausFilter->GetParam("blurScale")->SetAsFloats(&iwm::vector3(blurw, 0, 0), 3);
		Renderer.ApplyFilter(gausFilter, shadowTarget, shadowTargetBlur);

		gausFilter->GetParam("blurScale")->SetAsFloats(&iwm::vector3(0, blurh, 0), 3);
		Renderer.ApplyFilter(gausFilter, shadowTargetBlur, shadowTarget);

		Renderer.BeginScene(cam);

		Renderer.Device->SetPipeline(pbr.get());
		pbr->GetParam("lightPositions")->SetAsFloats(&lightPositions, 4, 3); // need better way to pass scene data
		pbr->GetParam("lightColors")->SetAsFloats(&lightColors, 4, 3);
		pbr->GetParam("camPos")->SetAsFloats(&controller->Camera->Position, 3);
		pbr->GetParam("lightSpace")->SetAsMat4(lightCam.GetViewProjection());
		//pbr->GetParam("sunDirection")->SetAsFloats(&lightPositions, 3);

		for (auto tree : Space.Query<IW::Transform, IwEngine::Model>()) {
			auto [transform, model] = tree.Components.Tie<TreeComponents>();

			for (size_t i = 0; i < model->MeshCount; i++) {
				Renderer.DrawMesh(transform, &model->Meshes[i]);
			}
		}

		Renderer.EndScene();
	}
}

void GameLayer3D::ImGui() {
	ImGui::Begin("Game layer");

	//ImGui::ColorPicker3("Color", (float*)std::get<0>(material->GetFloats("albedo")));
	//ImGui::SliderFloat("Metallic",  (float*)material->GetFloat("metallic"), 0, 1);
	//ImGui::SliderFloat("Roughness", (float*)material->GetFloat("roughness"), 0.3f, 1);
	//ImGui::SliderFloat("AO", (float*)material->GetFloat("ao"), 0, 1);
	//ImGui::ColorPicker4("Diffuse Color", (float*)&material->GetColor("diffuse"));
	//ImGui::ColorPicker4("Specular Color", (float*)&material->GetColor("specular"));

	ImGui::Text("Camera");
	ImGui::SliderFloat("Angle", &angle, 0, iwm::PI);
	ImGui::SliderFloat("Distance", &distance, 5, 500);
	ImGui::SliderFloat("Fov", &fov, 0.001f, iwm::PI / 4);

	ImGui::Text("Sun");
	ImGui::SliderFloat3("XYZ", &lightDirection.x, -1, 1);
	ImGui::SliderFloat("Blur", &blurAmount, 0.001f, 1.0f);

	ImGui::End();
}

bool GameLayer3D::On(
	IwEngine::MouseMovedEvent& event)
{
	//for (auto entity : Space.Query<IW::Transform, IwEngine::CameraController>()) {
	//	auto [transform, controller] = entity.Components.Tie<PlayerComponents>();

	//	float pitch = event.DeltaY * 0.0005f;
	//	float yaw   = event.DeltaX * 0.0005f;

	//	iwm::quaternion deltaP = iwm::quaternion::from_axis_angle(-transform->Right(), pitch);
	//	iwm::quaternion deltaY = iwm::quaternion::from_axis_angle( iwm::vector3::unit_y, yaw);

	//	transform->Rotation *= deltaP * deltaY;

	//	controller->Camera->Rotation = transform->Rotation;
	//}

	return false;
}

