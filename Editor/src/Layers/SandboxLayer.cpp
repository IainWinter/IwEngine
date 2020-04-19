#include "Layers/SandboxLayer.h"

#include "Events/ActionEvents.h"

#include "Systems/BulletSystem.h"
#include "Systems/LevelSystem.h"
#include "Systems/EnemyDeathCircleSystem.h"
#include "Systems/GameCameraController.h"
#include "Systems/ScoreSystem.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/ParticleUpdateSystem.h"
#include "iw/engine/Systems/ParticleUpdateSystem.h"
#include "iw/engine/Systems/Render/MeshRenderSystem.h"
#include "iw/engine/Systems/Render/ModelRenderSystem.h"
#include "iw/engine/Systems/Render/ParticleRenderSystem.h"
#include "iw/engine/Systems/Render/MeshShadowRenderSystem.h"
#include "iw/engine/Systems/Render/ModelShadowRenderSystem.h"
#include "iw/engine/Systems/Render/ParticleShadowRenderSystem.h"

#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Time.h"

#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/Rigidbody.h"

#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Font.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/TextureAtlas.h"

#include "iw/input/Devices/Mouse.h"
#include "iw/input/Devices/Keyboard.h"

#include "iw/audio/AudioSpaceStudio.h"

#include "imgui/imgui.h"

float randf() {
	return ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
}

namespace iw {
	struct ModelUBO {
		matrix4 model;
		vector3 albedo;
	};

	struct ModelComponents {
		Transform* Transform;
		Model* Model;
	};

	struct CameraComponents {
		Transform* Transform;
		CameraController* Controller;
	};

	struct PlaneComponents {
		Transform* Transform;
		PlaneCollider* Collider;
	};

	SandboxLayer::SandboxLayer()
		: Layer("Sandbox")
		, playerSystem(nullptr)
		, enemySystem(nullptr)
		, light(nullptr)
		, sun(nullptr)
		, textCam(nullptr)
		, textMesh(nullptr)
	{}

	int forestInstance = 0;

	iw::matrix4 persp = iw::matrix4::create_perspective_field_of_view(1.17f, 1.778f, 1.0f, 500.0f);
	iw::matrix4 ortho = iw::matrix4::create_orthographic(16 * 4, 9 * 4, -100, 100);
	float blend;

	int SandboxLayer::Initialize() {

		// Audio

		AudioSpaceStudio* studio = Audio->AsStudio();

		studio->SetVolume(0.25f);

		studio->LoadMasterBank("Master.bank");
		studio->LoadMasterBank("Master.strings.bank");
		studio->CreateEvent("swordAttack");
		studio->CreateEvent("forestAmbiance");
		studio->CreateEvent("playerDamaged");
		studio->CreateEvent("enemyDeath");

		forestInstance = studio->CreateInstance("forestAmbiance", false);

		// Fonts

		font = Asset->Load<Font>("fonts/arial.fnt");
		font->Initialize(Renderer->Device);

		textMesh = font->GenerateMesh("Use arrow keys to move and x to attack.\ni: debug menu\nt: freecam", .005f, 1);

		fontShader = Asset->Load<Shader>("shaders/font.shader");
		Renderer->InitShader(fontShader, CAMERA);

		ref<Material> textMat = REF<Material>(fontShader);

		textMat->Set("color", vector3::one);
		textMat->SetTexture("fontMap", font->GetTexture(0));

		textMat->Initialize(Renderer->Device);

		textMesh.SetMaterial(textMat);
		textMesh.Data()->Initialize(Renderer->Device);

		textTransform = Transform(vector3(-6.8, -1.8, 0), vector3::one, quaternion::identity);

		// Shaders

		ref<Shader> shader   = Asset->Load<Shader>("shaders/pbr.shader");
		ref<Shader> gaussian = Asset->Load<Shader>("shaders/filters/gaussian.shader");
		ref<Shader> dirShadowShader   = Asset->Load<Shader>("shaders/lights/directional.shader");
		ref<Shader> pointShadowShader = Asset->Load<Shader>("shaders/lights/point.shader");
		ref<Shader> dirIShadowShader   = Asset->Load<Shader>("shaders/lights/directional_instanced.shader");
		ref<Shader> pointIShadowShader = Asset->Load<Shader>("shaders/lights/point_instanced.shader");
		
		Renderer->InitShader(shader,   CAMERA | SHADOWS | LIGHTS);
		Renderer->InitShader(gaussian, CAMERA);
		Renderer->InitShader(dirShadowShader,  CAMERA);
		Renderer->InitShader(pointShadowShader);
		Renderer->InitShader(dirIShadowShader);
		Renderer->InitShader(pointIShadowShader);

		// Directional light shadow map textures & target

		ref<Texture> dirShadowColor = ref<Texture>(new Texture(1024, 1024, TEX_2D, RG,    FLOAT, BORDER));
		ref<Texture> dirShadowDepth = ref<Texture>(new Texture(1024, 1024, TEX_2D, DEPTH, FLOAT, BORDER));

		ref<RenderTarget> dirShadowTarget = REF<RenderTarget>(1024, 1024);
		dirShadowTarget->AddTexture(dirShadowColor);
		dirShadowTarget->AddTexture(dirShadowDepth);

		dirShadowTarget->Initialize(Renderer->Device);

		//ref<Texture> texBlur = REF<Texture>(1024, 1024, TEX_2D, ALPHA, FLOAT, BORDER);

		//ref<RenderTarget> targetBlur = REF<RenderTarget>(1024, 1024);
		//targetBlur->AddTexture(texBlur);
		//targetBlur->Initialize(Renderer->Device);

		// Point light shadow map textures & target

		ref<Texture> pointShadowDepth = ref<Texture>(new Texture(1024, 1024, TEX_CUBE, DEPTH, FLOAT, EDGE));

		ref<RenderTarget> pointShadowTarget = REF<RenderTarget>(1024, 1024, true);
		pointShadowTarget->AddTexture(pointShadowDepth);

		pointShadowTarget->Initialize(Renderer->Device);

		Asset->Give<iw::Texture>("SunShadowMap",   dirShadowTarget->Tex(0));
		Asset->Give<iw::Texture>("LightShadowMap", pointShadowTarget->Tex(0));

		// Scene 

		//	Lights

		sun   = new DirectionalLight(100, OrthographicCamera(60, 32, -100, 100), dirShadowTarget, dirShadowShader, dirIShadowShader);
		//light = new PointLight(30, 30, pointShadowTarget, pointShadowShader, pointIShadowShader);

		quaternion q = quaternion(0.872f, 0.0f, 0.303f, 0.384f);

		LOG_INFO << q.euler_angles(); // holy moly euler angles are wrong maths

		sun->SetRotation(q);
		//light->SetPosition(vector3(0, 10, 0));

		MainScene->AddLight(sun);
		//MainScene->AddLight(light);

		//	Cameras

		MainScene->SetMainCamera(new PerspectiveCamera()); // projection from up top
		textCam = new OrthographicCamera(vector3::one, quaternion::from_axis_angle(vector3::unit_y, Pi), 16, 9, -10, 10);

		iw::quaternion camrot = 
			  iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
			* iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::Pi);

		iw::Entity camera = Space->CreateEntity<iw::Transform, iw::CameraController>();

		iw::Transform* transform = camera.SetComponent<iw::Transform>(vector3(0, 27.18f, 0), iw::vector3::one, camrot);
		                           camera.SetComponent<iw::CameraController>(MainScene->MainCamera());

								   MainScene->MainCamera()->SetTrans(transform);

		// Materials

		Material* mat = new Material(shader);
		mat->Set("albedo", vector4(1, 1, 1, 1));
		mat->Set("roughness", 0.8f);
		mat->Set("metallic", 0.2f);
		mat->SetTexture("shadowMap",  dirShadowTarget->Tex(0));    // shouldnt really be part of material
		mat->SetTexture("shadowMap2", pointShadowTarget->Tex(0));
		mat->Initialize(Renderer->Device);

		// Models

		MeshDescription description;

		description.DescribeBuffer(bName::POSITION,  MakeLayout<float>(3));
		description.DescribeBuffer(bName::NORMAL,    MakeLayout<float>(3));
		description.DescribeBuffer(bName::TANGENT,   MakeLayout<float>(3));
		description.DescribeBuffer(bName::BITANGENT, MakeLayout<float>(3));
		description.DescribeBuffer(bName::UV,        MakeLayout<float>(2));

		Mesh smesh = MakeUvSphere(description, 25, 30)->MakeInstance();
		Mesh tmesh = MakeTetrahedron(description, 5)->MakeInstance();

		smesh.Data()->GenTangents();
		tmesh.Data()->GenTangents();

		smesh.Data()->Initialize(Renderer->Device);
		tmesh.Data()->Initialize(Renderer->Device);

		smesh.SetMaterial(mat->MakeInstance());
		tmesh.SetMaterial(mat->MakeInstance());

		Model smodel;
		smodel.AddMesh(smesh);
		
		Model tmodel;
		tmodel.AddMesh(tmesh);

		Asset->Give<Model>("Sphere",      &smodel);
		Asset->Give<Model>("Tetrahedron", &tmodel);

		//	Player

		Mesh pmesh = smesh.Data()->MakeInstance();
		pmesh.SetMaterial(mat->MakeInstance());

		Model pmodel;
		pmodel.AddMesh(pmesh);

		Asset->Give<Model>("Player", &pmodel);

		//	Door

		Mesh dmesh = MakeIcosphere(description, 0)->MakeInstance();
		dmesh.Data()->Initialize(Renderer->Device);
	
		dmesh.SetMaterial(mat->MakeInstance());
		dmesh.Material()->SetTransparency(Transparency::ADD);

		Model dmodel;
		dmodel.AddMesh(dmesh);

		Asset->Give<Model>("Door", &dmodel);

		// Physics

		Physics->SetGravity(vector3(0, -9.8f, 0));
		Physics->AddSolver(new ImpulseSolver());
		Physics->AddSolver(new SmoothPositionSolver());

		// Rendering pipeline
		//
		//generateShadowMap    = new GenerateShadowMap(Renderer, Space);
		//postProcessShadowMap = new FilterTarget(Renderer);
		//mainRender           = new Render(Renderer, Space);
		//
		//generateShadowMap   ->SetLight(light);
		//postProcessShadowMap->SetIntermediate(targetBlur);
		//postProcessShadowMap->SetShader(gaussian);
		//mainRender          ->SetLight(light);
		//mainRender          ->SetAmbiance(0.008f);
		//mainRender          ->SetGamma(2.2f);
		//
		//pipeline.first(generateShadowMap)
		//	.then(postProcessShadowMap)
		//	.then(mainRender);
		//
		// Systems

		// Systems

		playerSystem = PushSystem<PlayerSystem>();
		enemySystem  = PushSystem<EnemySystem>(playerSystem->GetPlayer());
		PushSystem<GameCameraController>(playerSystem->GetPlayer(), MainScene);
		PushSystem<BulletSystem>(playerSystem->GetPlayer());
		PushSystem<LevelSystem>(playerSystem->GetPlayer());
		PushSystem<ScoreSystem>(playerSystem->GetPlayer(), MainScene->MainCamera(), textCam);
		PushSystem<EnemyDeathCircleSystem>();
		PushSystem<PhysicsSystem>();


		PushSystem<iw::ParticleUpdateSystem>();

		PushSystem<iw::    MeshShadowRenderSystem>(MainScene);
		PushSystem<iw::   ModelShadowRenderSystem>(MainScene);
		PushSystem<iw::ParticleShadowRenderSystem>(MainScene);
		PushSystem<iw::          MeshRenderSystem>(MainScene);
		PushSystem<iw::         ModelRenderSystem>(MainScene);
		PushSystem<iw::      ParticleRenderSystem>(MainScene);

		// Particle test

		ref<Shader> particleShader = Asset->Load<Shader>("shaders/particle/simple.shader");
		Renderer->InitShader(particleShader);

		iw::Material particleMaterial(particleShader);
		particleMaterial.Set("color", Color::From255(0, 60, 10));
		particleMaterial.Initialize(Renderer->Device);

		Mesh particle = Asset->Load<Model>("models/forest/tuft_simple.dae")->GetMesh(0);
		particle.SetMaterial(particleMaterial.MakeInstance());

		iw::Entity particleEntity = Space->CreateEntity<iw::Transform, iw::ParticleSystem<StaticParticle>>();

		iw::Transform*                      t = particleEntity.SetComponent<iw::Transform>();
		iw::ParticleSystem<StaticParticle>* s = particleEntity.SetComponent<iw::ParticleSystem<StaticParticle>>();

		s->SetTransform(t);
		s->SetParticleMesh(particle);
		s->SetCamera(MainScene->MainCamera());

		s->SetUpdate([](auto s, auto p, auto c) {
			if (c < 3000 &&  Keyboard::KeyDown(G)) {
				for (int i = 0; i < 3000; i++) {
					float x = randf() * 32.0f;
					float z = randf() * 18.0f;

					if (   abs(x) > 28 + randf() * 2
						|| abs(z) > 12 + randf() * 2)
					{
						Transform trans;
						trans.Position.x = x;
						trans.Position.z = z;
						trans.Position.y = randf() * 0.5f + 0.25f;

						trans.Scale.x = (randf() + 1.2f) * 0.2;
						trans.Scale.z = (randf() + 1.2f) * 0.2;
						trans.Scale.y = (randf() + 1.5f) * 0.5;

						trans.Rotation = quaternion::from_euler_angles(0, randf() * 2 * Pi, 0);

						s->SpawnParticle(trans);
					}
				}

				return true;
			}

			return false;
		});

		return Layer::Initialize();
	}

	void SandboxLayer::PostUpdate() {
		// Update particle system

		//font->UpdateMesh(textMesh, std::to_string(1.0f / Time::DeltaTime()), 0.01f, 1); //fps
		if (textMesh.Data()->IsOutdated()) {
			textMesh.Data()->Update(Renderer->Device);
		}

		MainScene->MainCamera()->SetProjection(iw::lerp(persp, ortho, blend));
		
		// Shadow maps

		//for(Light* light : scene->DirectionalLights()) {
		//	if (!light->CanCastShadows()) {
		//		continue;
		//	}
		//
		//	Renderer->BeginShadowCast(light);
		//
		//	for (auto m_e : Space->Query<Transform, Model>()) {
		//		auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();
		//
		//		for (size_t i = 0; i < m_m->MeshCount; i++) {
		//			//iw::ref<Texture> t = m_m->Meshes[i].Material->GetTexture("alphaMaskMap");
		//
		//			//ITexture* it = nullptr;
		//			//if (t) {
		//			//	it = t->Handle();
		//			//}
		//
		//			//light->ShadowShader()->Handle()->GetParam("hasAlphaMask")->SetAsFloat(it != nullptr);
		//			//light->ShadowShader()->Handle()->GetParam("alphaMask")->SetAsTexture(it, 0);
		//			//light->ShadowShader()->Handle()->GetParam("alphaThreshold")->SetAsFloat(threshold);
		//
		//			Renderer->DrawMesh(m_t, &m_m->Meshes[i]);
		//		}
		//	}
		//
		//	Renderer->EndShadowCast();
		//}

		Renderer->BeginScene(textCam);
			Renderer->DrawMesh(textTransform, textMesh);
		Renderer->EndScene();

		//float blurw = 1.0f / target->Width() * blurAmount;
		//float blurh = 1.0f / target->Height() * blurAmount;

		//generateShadowMap->SetThreshold(threshold);
		//postProcessShadowMap->SetBlur(vector2(blurw, blurh));

		//pipeline.execute();
	}

	void SandboxLayer::ImGui() {
		ImGui::Begin("Sandbox");

		ImGui::SliderFloat("Ambiance", (float*)&MainScene->Ambiance(), 0, 1);
		//ImGui::SliderFloat("Gamma", (float*)&mainRender->GetGamma(), 0, 5);
		ImGui::SliderFloat("Camera blend", &blend, 0, 1);


		//ImGui::SliderFloat("Shadow map blur", &blurAmount, 0, 5);
		ImGui::SliderFloat("Shadow map threshold", &threshold, 0, 1);

		ImGui::SliderFloat3("Text pos",   (float*)&textTransform.Position, -8, 8);
		ImGui::SliderFloat3("Text scale", (float*)&textTransform.Scale, 0, 10);
		ImGui::SliderFloat4("Text rot",   (float*)&textTransform.Rotation, -1, 1);

		ImGui::SliderFloat("Volume", &Audio->GetVolume(), 0, 1);

		if (ImGui::Button("Start ambiance")) {
			Audio->AsStudio()->StartInstance(forestInstance);
		}

		if (ImGui::Button("Stop ambiance")) {
			Audio->AsStudio()->StopInstance(forestInstance);
		}

		if (ImGui::Button("Next level (don't spam)")) {
			Bus->push<LoadNextLevelEvent>();
		}

		//if (ImGui::Button("Save level")) {
		//	JsonSerializer jout("assets/levels/working.json", true);
		//	
		//	Level level;
		//	for (auto entity : Space->Query<Transform, Enemy>()) {
		//		auto [transform, enemy] = entity.Components.Tie<EnemySystem::Components>();
		//		level.Positions.push_back({ transform->Position.x, transform->Position.z });
		//		level.Enemies.push_back(*enemy);
		//	}

		//	level.StageName = "models/grass/grass.obj";
		//	
		//	jout.Write(level);
		//}

		//if (ImGui::Button("Load level")) {
		//	loadLevel = "assets/levels/forest/level1.bin";
		//}

		ImGui::End();
	}

	bool settexttocursor = false;

	bool SandboxLayer::On(
		MouseMovedEvent& e)
	{
		if (settexttocursor) {
			str.clear();
			font->UpdateMesh(textMesh, std::to_string((int)e.X) + '\n' + std::to_string((int)e.Y), .01f, 1);
		}

		if (e.X < -1000) {
			settexttocursor = true;
		}

		return false;
	}

	//bool SandboxLayer::On(
	//	KeyTypedEvent& e)
	//{
	//	str += e.Character;
	//	font->UpdateMesh(textMesh, str, .01f, 1);

	//	return false;
	//}

	bool SandboxLayer::On(
		ActionEvent& e)
	{
		switch (e.Action) {
			case iw::val(Actions::GOTO_NEXT_LEVEL): {
				playerSystem->On(e);
				enemySystem ->On(e);
				PopSystem(playerSystem);
				PopSystem(enemySystem);

				if (e.as<GoToNextLevelEvent>().LevelName == "models/block/forest100.dae") {
					font->UpdateMesh(textMesh, "ayy you've gotten to the boss congrats!\nsadly he's out today so\nhave some fun with the physics instead...\nmember you can press i/t", .004f, 1);
				}

				else if (e.as<GoToNextLevelEvent>().LevelName == "models/block/forest08.dae") {
					font->UpdateMesh(textMesh, "So this would be a lil mini boss but that seems\nlike it would be annoying to program xd", .004f, 1);
				}

				else if (e.as<GoToNextLevelEvent>().LevelName == "models/block/forest01.dae") {
					font->UpdateMesh(textMesh, "Lets go! You've finished the play test\nIf you got the time, post some feedback at\nhttps://winter.dev/demo", .004f, 1);
				}

				else {
					font->UpdateMesh(textMesh, "", .01f, 1);
					settexttocursor = false;
				}

				break;
			}
			case iw::val(Actions::AT_NEXT_LEVEL): {
				PushSystemFront(playerSystem);
				PushSystemFront(enemySystem);
				break;
			}
		}

		return Layer::On(e);
	}
}

