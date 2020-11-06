#include "Layers/SandboxLayer.h"

#include "Events/ActionEvents.h"

#include "Systems/BulletSystem.h"
#include "Systems/LevelSystem.h"
#include "Systems/LevelLayoutSystem.h"
#include "Systems/EnemyDeathCircleSystem.h"
#include "Systems/GameCameraController.h"
#include "Systems/ScoreSystem.h"
#include "Systems/ItemSystem.h"
#include "Systems/NoteSystem.h"
#include "Systems/ConsumableSystem.h"
#include "Systems/SpaceInspectorSystem.h"
#include "Systems/WorldHoleSystem.h"
#include "Systems/WorldLadderSystem.h"
#include "Systems/GameSaveStateSystem.h"
#include "Systems/SpecialBarrierSystem.h"
#include "Systems/RiverBarrierSystem.h"
#include "Systems/RiverRaftEnemySystem.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/ParticleUpdateSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/engine/Systems/Render/RenderSystem.h"
#include "iw/engine/Systems/Render/ShadowRenderSystem.h"
#include "iw/engine/Systems/Render/ModelVoxelRenderSystem.h"
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

#include "iw/entity/Prefab.h"

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

	ref<Shader> fontShader;

	int SandboxLayer::Initialize() {
		// Audio

		AudioSpaceStudio* studio = Audio->AsStudio();

		Renderer->Device->Clear();

		studio->SetVolume(0.25f);

		studio->LoadMasterBank("Master.bank");
		studio->LoadMasterBank("Master.strings.bank");
		studio->CreateEvent("Ambiance/forest");
		studio->CreateEvent("User/playerAttack");
		studio->CreateEvent("User/playerDamaged");
		studio->CreateEvent("User/enemyDeath");
		studio->CreateEvent("Music/boss");

		forestInstance = studio->CreateInstance("Ambiance/forest", false);

		// Fonts

		m_font = Asset->Load<Font>("fonts/arial.fnt");
		m_font->Initialize(Renderer->Device);

		fontShader = Asset->Load<Shader>("shaders/font_simple.shader");
		Renderer->InitShader(fontShader, CAMERA);

		ref<Material> textMat = REF<Material>(fontShader);
		textMat->Set("color", iw::Color(1));
		textMat->SetTexture("fontMap", m_font->GetTexture(0));

		Asset->Give<Material>("materials/Font", textMat);

		Mesh textMesh = m_font->GenerateMesh("(temp)\nr: restart level  i: debug menu  t: freecam\n e: show voxels (goodbye fps)\nfeedback: https://winter.dev/demo", .005f, 1);
		textMesh.SetMaterial(textMat);

		iw::Entity textEnt = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();

		             textEnt.Set<iw::Transform>(vector3(-7.8, -2.8, -5));
		m_textMesh = textEnt.Set<iw::Mesh>(textMesh);

		// Shaders


		            vct                = Asset->Load<Shader>("shaders/vct/vct.shader");
		ref<Shader> phong              = Asset->Load<Shader>("shaders/phong.shader");
		ref<Shader> gaussian           = Asset->Load<Shader>("shaders/filters/gaussian.shader");
		ref<Shader> dirShadowShader    = Asset->Load<Shader>("shaders/lights/directional_transparent.shader");
		//ref<Shader> pointShadowShader  = Asset->Load<Shader>("shaders/lights/point.shader");
		ref<Shader> dirIShadowShader   = Asset->Load<Shader>("shaders/lights/directional_instanced.shader");
		//ref<Shader> pointIShadowShader = Asset->Load<Shader>("shaders/lights/point_instanced.shader");
		
		Renderer->InitShader(vct,   CAMERA | SHADOWS | LIGHTS);
		Renderer->InitShader(phong, CAMERA | SHADOWS | LIGHTS);
		Renderer->InitShader(gaussian, CAMERA);
		Renderer->InitShader(dirShadowShader,  CAMERA);
		//Renderer->InitShader(pointShadowShader);
		Renderer->InitShader(dirIShadowShader, CAMERA);
		//Renderer->InitShader(pointIShadowShader);
		
		Renderer->SetShader(vct);
		//vct->Handle()->GetParam("SHADOWS")->SetAsInt(0);

		// Directional light shadow map textures & target

		ref<Texture> dirShadowColor = ref<Texture>(new Texture(1024, 1024, TEX_2D, RG,    FLOAT, BORDER));
		ref<Texture> dirShadowDepth = ref<Texture>(new Texture(1024, 1024, TEX_2D, DEPTH, FLOAT, BORDER));

		ref<RenderTarget> dirShadowTarget = REF<RenderTarget>();
		dirShadowTarget->AddTexture(dirShadowColor);
		dirShadowTarget->AddTexture(dirShadowDepth);

		dirShadowTarget->Initialize(Renderer->Device);

		//ref<Texture> texBlur = REF<Texture>(1024, 1024, TEX_2D, ALPHA, FLOAT, BORDER);

		//ref<RenderTarget> targetBlur = REF<RenderTarget>();
		//targetBlur->AddTexture(texBlur);
		//targetBlur->Initialize(Renderer->Device);

		// Point light shadow map textures & target

		//ref<Texture> pointShadowDepth = ref<Texture>(new Texture(1024, 1024, TEX_CUBE, DEPTH, FLOAT, EDGE));

		//ref<RenderTarget> pointShadowTarget = REF<RenderTarget>(true);
		//pointShadowTarget->AddTexture(pointShadowDepth);

		//pointShadowTarget->Initialize(Renderer->Device);

		Asset->Give<iw::Texture>("SunShadowMap",   dirShadowTarget->Tex(0));
		//Asset->Give<iw::Texture>("LightShadowMap", pointShadowTarget->Tex(0));

		// Scene 

		//	Lights

		sun   = new DirectionalLight(100, OrthographicCamera(60, 60, -100, 100), dirShadowTarget, dirShadowShader, dirIShadowShader);
		//light = new PointLight(30, 30, nullptr, nullptr, nullptr);

		sun->SetColor(vector3(.241f, .102f, .034f) * 2);
		sun->SetRotation(quaternion::from_euler_angles(1.338f, 0.0f, -0.836f));
		//light->SetPosition(vector3(0, 5, 0));

		MainScene->AddLight(sun);
		//MainScene->AddLight(light);

		//	Cameras

		MainScene->SetMainCamera(new PerspectiveCamera()); // projection from up top
		m_textCam = new OrthographicCamera(0, quaternion::from_axis_angle(vector3::unit_y, Pi), 16, 9, -10, 10);

		iw::quaternion camrot = 
			  iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
			* iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::Pi);

		iw::Entity camera = Space->CreateEntity<iw::Transform, iw::CameraController>();

		iw::Transform* transform = camera.Set<iw::Transform>(vector3(0, 27.18f, 0), iw::vector3::one, camrot);
		                           camera.Set<iw::CameraController>(MainScene->MainCamera());

		MainScene->MainCamera()->SetTrans(transform);

		// Materials

		Material* def = new Material(vct);
		def->Set("baseColor", vector4(0.8f, 1.0f));
		//def->Set("roughness", 0.8f);
		//def->Set("metallic", 0.2f);
		def->Set("reflectance", 1.0f);
		def->Set("emissive", 0.0f);
		def->Set("indirectDiffuse",  1);
		def->Set("indirectSpecular", 1);
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

		Mesh smesh = MakeIcosphere  (description, 5)->MakeInstance();
		Mesh tmesh = MakeTetrahedron(description, 5)->MakeInstance();
		Mesh bmesh = MakeCube       (description)   ->MakeInstance();

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

		// Systems

		GameSaveStateSystem* saveSystem = PushSystem<GameSaveStateSystem>();

								   PushSystem<LevelLayoutSystem>();
		LevelSystem* levelSystem = PushSystem<LevelSystem>();

		playerSystem    = PushSystem<PlayerSystem>(levelSystem->GetWorldTransform());
		bulletSystem    = PushSystem<BulletSystem>(playerSystem->GetPlayer());
		enemySystem     = PushSystem<EnemySystem>(playerSystem->GetPlayer(), bulletSystem->GetBulletPrefab());
		enemyBossSystem = PushSystem<EnemyBossSystem>(playerSystem->GetPlayer(), enemySystem, saveSystem->GetState());

		PushSystem<RiverBarrierSystem>(playerSystem->GetPlayer());
		PushSystem<RiverRaftEnemySystem>();
		
		PushSystem<WorldHoleSystem>();
		PushSystem<WorldLadderSystem>(saveSystem->GetState());
		PushSystem<SpecialBarrierSystem>(playerSystem->GetPlayer());


		PushSystem<GameCameraController>(playerSystem->GetPlayer());
		PushSystem<ScoreSystem>(playerSystem->GetPlayer(), MainScene->MainCamera(), m_textCam);
		PushSystem<EnemyDeathCircleSystem>();
		PushSystem<PhysicsSystem>();

		PushSystem<ItemSystem>(saveSystem->GetState());
		PushSystem<NoteSystem>();
		PushSystem<ConsumableSystem>(playerSystem->GetPlayer());

		PushSystem<iw::ParticleUpdateSystem>();

		PushSystem<iw::        ShadowRenderSystem>(MainScene);
		PushSystem<iw::ParticleShadowRenderSystem>(MainScene);
		PushSystem<iw::              RenderSystem>(MainScene);
		PushSystem<iw::      ParticleRenderSystem>(MainScene);

		PushSystem<iw::UiRenderSystem>(m_textCam);

		PushSystem<iw::ModelVoxelRenderSystem>(MainScene);

		PushSystem<SpaceInspectorSystem>();

		PushSystem<iw::EntityCleanupSystem>();

		return Layer::Initialize();
	}

	float f = 0;
	bool s = 0;
	bool b = 1;

	float ml = 2.0f;

	iw::vector3 skyColor;
	float w123 = 0.5f;
	float e123 = 1.0f;

	float ambiance = 0.03f;

	//float tick = 0;
	//int iiii = 0;

	void SandboxLayer::PostUpdate() {
		//srand(3);

		//std::vector<std::function<float(vector3 pos,
		//	size_t x, size_t y, size_t z,
		//	float*** weights)>> list;

		//if (iw::TotalTime() - tick > 1) {
		//	tick = iw::TotalTime();
		//	iiii++;

		//	auto sphere = [](
		//		vector3 pos,
		//		size_t, size_t y, size_t,
		//		float***)
		//	{
		//		return y == 1 /*|| 1 - pos.length()*/ > 0 ? 1 : 0;
		//	};

		//	auto jitter = [](
		//		vector3 pos,
		//		size_t x, size_t y, size_t z,
		//		float*** weights)
		//	{
		//		float left  = weights[x-1][y]  [z];
		//		float right = weights[x+1][y]  [z];
		//		float bot   = weights[x]  [y-1][z];
		//		float top   = weights[x]  [y+1][z];
		//		float front = weights[x]  [y]  [z-1];
		//		float back  = weights[x]  [y]  [z+1];

		//		if (   left + right + front + back == 1
		//			&& weights[x][y-1][z]          < 0)          return 0.0f;

		//		if (left + right + bot + top + front + back < 0) return 0.0f;

		//			 if (randf() > 0.9f) return left;
		//		else if (randf() > 0.9f) return right;
		//		else if (randf() > 0.9f) return front;
		//		else if (randf() > 0.9f) return back;
		//		else if (randf() > 0) return top;
		//		else if (randf() > 0) return bot;

		//		return 0.0f;
		//	};

		//	MeshDescription d;
		//	d.DescribeBuffer(bName::POSITION, iw::MakeLayout<float>(3));
		//	d.DescribeBuffer(bName::NORMAL, iw::MakeLayout<float>(3));

		//	list.push_back(sphere);
		//	for (int i = 0; i < iiii; i++) {
		//		list.push_back(jitter);
		//	}

		//	playerSystem->GetPlayer().Find<iw::Model>()->GetMesh(0).SetData(iw::GenerateFromVoxels(
		//		d, 0, vector3(30, 30, 30), .5,
		//		list)->MakeLink());
		//}

		// Update particle system

		//font->UpdateMesh(textMesh, std::to_string(1.0f / Time::DeltaTime()), 0.01f, 1); //fps
		seq.update();

		MainScene->MainCamera()->SetProjection(iw::lerp(persp, ortho, blend));

		MainScene->SetAmbiance(ambiance);

		vector3 camPos = MainScene->MainCamera()->WorldPosition(); // no req
		camPos.y = 0;

		sun->SetPosition(camPos);

		//fontShader->Use(Renderer->Device);

		//fontShader->Handle()->GetParam("width")->SetAsFloat(w123);
		//fontShader->Handle()->GetParam("edge") ->SetAsFloat(e123);

		vct->Use(Renderer->Device);

		//ml = iw::lerp(ml, 1 / iw::DeltaTime() / 40, iw::Time::DeltaTime() * .1f);
		vct->Handle()->GetParam("maxConeLength")->SetAsFloat(ml);
		vct->Handle()->GetParam("skyColor")->SetAsFloats(&skyColor, 3);


		if (Keyboard::KeyDown(V) && f > 0.2f) {
			vct->Handle()->GetParam("SHADOWS")->SetAsInt(s = !s);
			f = 0.0f;
		}

		if (Keyboard::KeyDown(B) && f > 0.2f) {
			vct->Handle()->GetParam("BLINN")  ->SetAsInt(b = !b);
			f = 0.0f;
		}

		f += Time::DeltaTime();
	}

	void SandboxLayer::ImGui() {
		ImGui::Begin("Sandbox");

		ImGui::Text("Events %i", Bus->count());

		ImGui::SliderFloat("Ambiance", (float*)&ambiance, 0, 1);
		//ImGui::SliderFloat("Gamma", (float*)&mainRender->GetGamma(), 0, 5);
		ImGui::SliderFloat("Camera blend", &blend, 0, 1);

		ImGui::DragFloat("Max Cone Length %f", &ml);
		ImGui::SliderFloat3("Sky color %f", (float*)&skyColor, 0, 1);

		//ImGui::SliderFloat("Shadow map blur", &blurAmount, 0, 5);
		ImGui::SliderFloat("Shadow map threshold", &threshold, 0, 1);


		float time = Time::TimeScale();
		ImGui::SliderFloat("Timescale", &time, 0, 2);

		if (time != Time::TimeScale()) {
			Time::SetTimeScale(time);
		}

		ImGui::SliderFloat("Volume", &Audio->GetVolume(), 0, 1);

		ImGui::DragFloat("Font width", &w123, .01f);
		ImGui::DragFloat("Font edge",  &e123, .01f);

		if (ImGui::Button("Start ambiance")) {
			Audio->AsStudio()->StartInstance(forestInstance);
		}

		if (ImGui::Button("Stop ambiance")) {
			Audio->AsStudio()->StopInstance(forestInstance);
		}

		if (ImGui::Button("Next level (don't spam)")) {
			Bus->push<GotoConnectedLevelEvent>(1);
		}

		if (ImGui::Button("Spawn slowmo")) {
			Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 0 }, playerSystem->GetPlayer().Find<iw::Transform>()->WorldPosition(), nullptr);
		}

		ImGui::SameLine();
		
		if (ImGui::Button("Spawn charge")) {
			Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 1 }, playerSystem->GetPlayer().Find<iw::Transform>()->WorldPosition(), nullptr);
		}

		ImGui::SameLine();

		if (ImGui::Button("Spawn dash")) {
			Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 2 }, playerSystem->GetPlayer().Find<iw::Transform>()->WorldPosition(), nullptr);
		}

		if (ImGui::Button("Start seq")) {
			seq.restart();
		}

		ImGui::End();

		Layer::ImGui();
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

	bool SandboxLayer::On(
		ActionEvent& e)
	{
		switch (e.Action) {
			//case iw::val(Actions::GOTO_LEVEL):
			//case iw::val(Actions::GOTO_CONNECTED_LEVEL): {
			//	enemySystem->On(e);
			//	PopSystem(enemySystem);
			//	PopSystem(enemyBossSystem);

			//	break;
			//}
			//case iw::val(Actions::AT_NEXT_LEVEL): {
			//	//PushSystemFront(playerSystem);
			//	PushSystemFront(enemySystem);
			//	PushSystemFront(enemyBossSystem);

			//	break;
			//}
			case iw::val(Actions::GAME_STATE): {
				GameStateEvent& event = e.as<GameStateEvent>();

				if (event.State == RUNNING) {
					PushSystemFront(playerSystem);
					PushSystemFront(enemySystem);
					PushSystemFront(enemyBossSystem);
					PushSystemFront(bulletSystem);
				}

				else if (event.State == PAUSED) {
					PopSystem(playerSystem);
					PopSystem(enemySystem);
					PopSystem(enemyBossSystem);
					PopSystem(bulletSystem);
				}

				break;
			}
		}

		if (e.Action == iw::val(Actions::START_LEVEL)) {
			std::string_view name = e.as<StartLevelEvent>().LevelName;

			m_font->UpdateMesh(*m_textMesh, "", .01f, 1);
			settexttocursor = false;

			ml = 2.0f;
			ambiance = 0.03f;

			if (name.find("canyon") != std::string::npos) {
				sun->SetRotation(iw::quaternion::from_euler_angles(1.0f, 0.0f, -0.35f));
				sun->SetColor(vector3(1.0f, 0.64f, 0.37f) * 0.33f);

				if (   name.find("cave") != std::string::npos
					|| name.find(".a")    != std::string::npos)
				{
					sun->SetRotation(iw::quaternion::from_euler_angles(1.4f, 0.0f, -0.25f));
					sun->SetColor(vector3(1.0f, 0.64f, 0.37f));

					// set shadow dimension

					ml = 8.0f;
					ambiance = 0.003f; //MainScene->SetAmbiance(0.0003f);
				}
			}

			else if (name.find("river") != std::string::npos) {
				sun->SetRotation(iw::quaternion::from_euler_angles(2, 0, 0.9f));
				sun->SetColor(vector3(0.665387f, 0.665387f, 1.0f) * 0.041f);

				ml = 8.0f;
				ambiance = 0.006f;
			}
		}

		return Layer::On(e);
	}
}

