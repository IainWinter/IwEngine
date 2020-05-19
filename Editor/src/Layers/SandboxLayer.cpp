#include "Layers/SandboxLayer.h"

#include "Events/ActionEvents.h"

#include "Systems/BulletSystem.h"
#include "Systems/LevelSystem.h"
#include "Systems/EnemyDeathCircleSystem.h"
#include "Systems/GameCameraController.h"
#include "Systems/ScoreSystem.h"
#include "Systems/ItemSystem.h"
#include "Systems/NoteSystem.h"
#include "Systems/ConsumableSystem.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/ParticleUpdateSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/engine/Systems/Render/MeshRenderSystem.h"
#include "iw/engine/Systems/Render/MeshShadowRenderSystem.h"
#include "iw/engine/Systems/Render/ModelRenderSystem.h"
#include "iw/engine/Systems/Render/ModelVoxelRenderSystem.h"
#include "iw/engine/Systems/Render/ModelShadowRenderSystem.h"
#include "iw/engine/Systems/Render/ParticleRenderSystem.h"
#include "iw/engine/Systems/Render/ParticleShadowRenderSystem.h"
#include "iw/engine/Systems/Render/UiRenderSystem.h"
#include "iw/engine/Systems/Debug/DrawCollidersSystem.h"

#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Time.h"

#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/PositionSolver.h"
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

#include "iw/events/seq/event_seq.h"

#include "iw/engine/Events/Seq/MoveToTarget.h"
#include "iw/engine/Events/Seq/DestroyEntity.h"

namespace iw {
	struct ModelUBO {
		matrix4 model;
		vector3 diffuse;
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
		, m_textCam(nullptr)
		, m_textMesh(nullptr)
	{}

	int forestInstance = 0;

	iw::matrix4 persp = iw::matrix4::create_perspective_field_of_view(1.17f, 1.778f, 1.0f, 500.0f);
	iw::matrix4 ortho = iw::matrix4::create_orthographic(16 * 4, 9 * 4, -100, 100);
	float blend;

	iw::event_seq seq;

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

		m_font = Asset->Load<Font>("fonts/arial.fnt");
		m_font->Initialize(Renderer->Device);

		ref<Shader> fontShader = Asset->Load<Shader>("shaders/font.shader");
		Renderer->InitShader(fontShader, CAMERA);

		ref<Material> textMat = REF<Material>(fontShader);
		textMat->Set("color", vector3(1));
		textMat->SetTexture("fontMap", m_font->GetTexture(0));

		Mesh textMesh = m_font->GenerateMesh("Use arrow keys to move and x to attack.\ni: debug menu\nt: freecam", .005f, 1);
		textMesh.SetMaterial(textMat);

		iw::Entity textEnt = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();

		             textEnt.Set<iw::Transform>(vector3(-6.8, -1.8, 0), vector3(1), quaternion::identity);
		m_textMesh = textEnt.Set<iw::Mesh>(textMesh);

		// Shaders


		ref<Shader> shader   = Asset->Load<Shader>("shaders/phong.shader");
		ref<Shader> gaussian = Asset->Load<Shader>("shaders/filters/gaussian.shader");
		ref<Shader> dirShadowShader    = Asset->Load<Shader>("shaders/lights/directional.shader");
		ref<Shader> pointShadowShader  = Asset->Load<Shader>("shaders/lights/point.shader");
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

		//ref<Texture> pointShadowDepth = ref<Texture>(new Texture(1024, 1024, TEX_CUBE, DEPTH, FLOAT, EDGE));

		//ref<RenderTarget> pointShadowTarget = REF<RenderTarget>(1024, 1024, true);
		//pointShadowTarget->AddTexture(pointShadowDepth);

		//pointShadowTarget->Initialize(Renderer->Device);

		Asset->Give<iw::Texture>("SunShadowMap",   dirShadowTarget->Tex(0));
		//Asset->Give<iw::Texture>("LightShadowMap", pointShadowTarget->Tex(0));

		// Scene 

		//	Lights

		sun   = new DirectionalLight(100, OrthographicCamera(60, 60, -100, 100), dirShadowTarget, dirShadowShader, dirIShadowShader);
		//light = new PointLight(30, 30, pointShadowTarget, pointShadowShader, pointIShadowShader);

		sun->SetRotation(quaternion::from_euler_angles(1.433f, 0.0f, -0.525f));
		//light->SetPosition(vector3(0, 10, 0));

		MainScene->AddLight(sun);
		//MainScene->AddLight(light);

		//	Cameras

		MainScene->SetMainCamera(new PerspectiveCamera()); // projection from up top
		m_textCam = new OrthographicCamera(vector3::one, quaternion::from_axis_angle(vector3::unit_y, Pi), 16, 9, -10, 10);

		iw::quaternion camrot = 
			  iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
			* iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::Pi);

		iw::Entity camera = Space->CreateEntity<iw::Transform, iw::CameraController>();

		iw::Transform* transform = camera.Set<iw::Transform>(vector3(0, 27.18f, 0), iw::vector3::one, camrot);
		                           camera.Set<iw::CameraController>(MainScene->MainCamera());

		MainScene->MainCamera()->SetTrans(transform);

		// Materials

		Material* def = new Material(shader);
		def->Set("baseColor", vector4(1, 1, 1, 1));
		def->Set("roughness", 0.8f);
		def->Set("metallic", 0.2f);
		def->Set("reflectance", 0.2f);
		def->SetTexture("shadowMap",  dirShadowTarget->Tex(0));    // shouldnt really be part of material
		//def->SetTexture("shadowMap2", pointShadowTarget->Tex(0));
		def->Initialize(Renderer->Device);

		ref<Material> mat = Asset->Give<iw::Material>("materials/Default", def);

		// Models

		MeshDescription description;

		description.DescribeBuffer(bName::POSITION,  MakeLayout<float>(3));
		description.DescribeBuffer(bName::NORMAL,    MakeLayout<float>(3));
		description.DescribeBuffer(bName::TANGENT,   MakeLayout<float>(3));
		description.DescribeBuffer(bName::BITANGENT, MakeLayout<float>(3));
		description.DescribeBuffer(bName::UV,        MakeLayout<float>(2));

		Mesh smesh = MakeUvSphere(description, 25, 30)->MakeInstance();
		Mesh tmesh = MakeTetrahedron(description, 5)->MakeInstance();
		Mesh bmesh = MakeCube(description)->MakeInstance();

		smesh.Data()->GenTangents();
		tmesh.Data()->GenTangents();
		bmesh.Data()->GenTangents();

		smesh.SetMaterial(mat->MakeInstance());
		tmesh.SetMaterial(mat->MakeInstance());
		bmesh.SetMaterial(mat->MakeInstance());

		Model smodel;
		smodel.AddMesh(smesh);
		
		Model tmodel;
		tmodel.AddMesh(tmesh);

		Model bmodel;
		bmodel.AddMesh(bmesh);

		Asset->Give<Model>("Sphere", &smodel);
		Asset->Give<Model>("Tetrahedron", &tmodel);
		Asset->Give<Model>("Box", &bmodel);

		//	Player

		Mesh pmesh = smesh.Data()->MakeInstance();
		pmesh.SetMaterial(mat->MakeInstance());

		Model pmodel;
		pmodel.AddMesh(pmesh);

		Asset->Give<Model>("Player", &pmodel);

		//	Door

		Mesh dmesh = MakeIcosphere(description, 0)->MakeInstance();
	
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
		bulletSystem = PushSystem<BulletSystem>(playerSystem->GetPlayer());

		PushSystem<GameCameraController>(playerSystem->GetPlayer(), MainScene);
		PushSystem<LevelSystem>(playerSystem->GetPlayer());
		PushSystem<ScoreSystem>(playerSystem->GetPlayer(), MainScene->MainCamera(), m_textCam);
		PushSystem<EnemyDeathCircleSystem>();
		PushSystem<PhysicsSystem>();

		PushSystem<ItemSystem>();
		PushSystem<NoteSystem>();
		PushSystem<ConsumableSystem>(playerSystem->GetPlayer());

		PushSystem<iw::ParticleUpdateSystem>();
		PushSystem<iw::EntityCleanupSystem>();

		//PushSystem<iw::ModelVoxelRenderSystem>(MainScene);

		PushSystem<iw::    MeshShadowRenderSystem>(MainScene);
		PushSystem<iw::   ModelShadowRenderSystem>(MainScene);
		PushSystem<iw::ParticleShadowRenderSystem>(MainScene);
		PushSystem<iw::          MeshRenderSystem>(MainScene);
		PushSystem<iw::         ModelRenderSystem>(MainScene);
		PushSystem<iw::      ParticleRenderSystem>(MainScene);

		PushSystem<iw::UiRenderSystem>(m_textCam);

		//PushSystem<iw::DrawCollidersSystem>(MainScene->MainCamera());

		// Particle test

		ref<Shader> particleShader = Asset->Load<Shader>("shaders/particle/simple.shader");
		Renderer->InitShader(particleShader);

		iw::Material particleMaterial(particleShader);
		particleMaterial.Set("color", Color::From255(0, 60, 10));
		particleMaterial.Initialize(Renderer->Device);

		Mesh particle = Asset->Load<Model>("models/forest/tuft_simple.dae")->GetMesh(0);
		particle.SetMaterial(particleMaterial.MakeInstance());

		iw::Entity particleEntity = Space->CreateEntity<iw::Transform, iw::ParticleSystem<StaticParticle>>();

		iw::Transform*                      t = particleEntity.Set<iw::Transform>();
		iw::ParticleSystem<StaticParticle>* s = particleEntity.Set<iw::ParticleSystem<StaticParticle>>();

		s->SetTransform(t);
		s->SetParticleMesh(particle);
		s->SetCamera(MainScene->MainCamera());

		s->SetUpdate([](auto s, auto p, auto c) {
			if (c < 3000 &&  Keyboard::KeyDown(G)) {
				for (int i = 0; i < 3000; i++) {
					float x = iw::randf() * 32.0f;
					float z = iw::randf() * 18.0f;

					if (   abs(x) > 28.0f + randf() * 2.0f
						|| abs(z) > 12.0f + randf() * 2.0f)
					{
						Transform trans;
						trans.Position.x = x;
						trans.Position.z = z;
						trans.Position.y = randf() * 0.5f + 0.25f;

						trans.Scale.x = (randf() + 1.2f) * 0.2f;
						trans.Scale.z = (randf() + 1.2f) * 0.2f;
						trans.Scale.y = (randf() + 1.5f) * 0.5f;

						trans.Rotation = quaternion::from_euler_angles(0, randf() * 2.0f * Pi, 0.0f);

						s->SpawnParticle(trans);
					}
				}

				return true;
			}

			return false;
		});

		//iw::ref<iw::Model> rock = Asset->Load<iw::Model>("models/forest/ground.dae");

		//rock->GetMesh(0).Material()->SetShader(shader);

		//iw::Entity e = Space->CreateEntity<iw::Transform, iw::Model>();

		//e.Set<iw::Transform>();
		//e.Set<iw::Model>(*rock);

		iw::MoveToTarget* move1 = new iw::MoveToTarget(playerSystem->GetPlayer(), iw::vector3(-10, 1, 0));
		iw::MoveToTarget* move2 = new iw::MoveToTarget(playerSystem->GetPlayer(), iw::vector3(-10, 1, 10));
		seq.add(move1);
		seq.add(move2);

		//iw::DestroyEntity* des = new iw::DestroyEntity(playerSystem->GetPlayer());
		//seq.add(des);
	
		return Layer::Initialize();
	}

	void SandboxLayer::PostUpdate() {
		// Update particle system

		//font->UpdateMesh(textMesh, std::to_string(1.0f / Time::DeltaTime()), 0.01f, 1); //fps
		seq.update();

		MainScene->MainCamera()->SetProjection(iw::lerp(persp, ortho, blend));
		
		//Renderer->BeginScene(MainScene, voxelRT);

		//for (auto e : Space->Query<iw::Transform, iw::Model>()) {
		//	auto [t, m] = e.Components.Tie<ModelComponents>();

		//	for (Mesh& mesh : m->GetMeshes()) {
		//		//Renderer->BeforeDraw([&]() {
		//		//	(*Renderer).SetShader(voxelize);
		//		//});

		//		Renderer->DrawMesh(t, &mesh);
		//	}
		//}

		//Renderer->EndScene();

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

		if (ImGui::Button("Spawn slowmo")) {
			Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 0 }, iw::vector3(0, 0.1f, 8), nullptr);
		}

		if (ImGui::Button("Start seq")) {
			seq.restart();
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
			m_font->UpdateMesh(*m_textMesh, std::to_string((int)e.X) + '\n' + std::to_string((int)e.Y), .01f, 1);
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
				//playerSystem->On(e);
				enemySystem ->On(e);
				//PopSystem(playerSystem);
				PopSystem(enemySystem);

				if (e.as<GoToNextLevelEvent>().LevelName == "models/block/forest100.json") {
					m_font->UpdateMesh(*m_textMesh, "ayy you've gotten to the boss congrats!\nsadly he's out today so\nhave some fun with the physics instead...\nmember you can press i/t", .004f, 1);
				}

				else if (e.as<GoToNextLevelEvent>().LevelName == "models/block/forest08.json") {
					m_font->UpdateMesh(*m_textMesh, "So this would be a lil mini boss but that seems\nlike it would be annoying to program xd", .004f, 1);
				}

				else if (e.as<GoToNextLevelEvent>().LevelName == "models/block/forest01.json") {
					m_font->UpdateMesh(*m_textMesh, "Lets go! You've finished the play test\nIf you got the time, post some feedback at\nhttps://winter.dev/demo", .004f, 1);
				}

				else {
					m_font->UpdateMesh(*m_textMesh, "", .01f, 1);
					settexttocursor = false;
				}

				break;
			}
			case iw::val(Actions::AT_NEXT_LEVEL): {
				//PushSystemFront(playerSystem);
				PushSystemFront(enemySystem);

				break;
			}
			case iw::val(Actions::GAME_STATE): {
				GameStateEvent& event = e.as<GameStateEvent>();

				if (event.State == RUNNING) {
					PushSystemFront(playerSystem);
					PushSystemFront(enemySystem);
					PushSystemFront(bulletSystem);
				}

				else {
					PopSystem(playerSystem);
					PopSystem(enemySystem);
					PopSystem(bulletSystem);
				}

				break;
			}
		}

		return Layer::On(e);
	}
}

