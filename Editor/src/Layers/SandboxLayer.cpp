#include "Layers/SandboxLayer.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"
#include "Systems/BulletSystem.h"
#include "Systems/LevelSystem.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Systems/PhysicsSystem.h"
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
#include "iw/audio/AudioSpaceStudio.h"
#include "imgui/imgui.h"

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
	{}

	int forestInstance = 0;

	int SandboxLayer::Initialize() {
		AudioSpaceStudio* studio = (AudioSpaceStudio*)Audio->AsStudio();

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

		textMesh = font->GenerateMesh("Winter.dev", .01f, 1);

		fontShader = Asset->Load<Shader>("shaders/font.shader");
		Renderer->InitShader(fontShader, CAMERA);

		ref<Material> textMat = REF<Material>(fontShader);

		textMat->Set("color", vector3::one);
		textMat->SetTexture("fontMap", font->GetTexture(0));

		textMesh->SetMaterial(textMat);
		textMesh->Initialize(Renderer->Device);

		textTransform = { vector3(-6.8, -1.8, 0), vector3::one, quaternion::identity};

		// Shader

		ref<Shader> shader      = Asset->Load<Shader>("shaders/pbr.shader");
		ref<Shader> directional = Asset->Load<Shader>("shaders/lights/directional_transparent.shader");
		            gaussian    = Asset->Load<Shader>("shaders/filters/gaussian.shader");
		
		Renderer->InitShader(shader,      ALL);
		Renderer->InitShader(directional, CAMERA);
		Renderer->InitShader(gaussian,    CAMERA);

		Renderer->SetShader(shader);

		vector3* positions = new vector3[2];
		vector3* colors    = new vector3[2];

		positions[0] = vector3(3, 3, 0);
		positions[1] = vector3(-3, 3, 0);

		colors[0] = vector3(1);
		colors[1] = vector3(1);

		shader->Program->GetParam("lightPositions[0]")->SetAsFloats(positions, 3, 2);
		shader->Program->GetParam("lightColors[0]")   ->SetAsFloats(colors, 3, 2);

		// Textures
		
		Texture shadow = Texture(1024, 1024, RG,    FLOAT, BORDER);
		ref<Texture> texShadow = Asset->Give("ShadowMap", &shadow);
		
		ref<Texture> texDepth  = REF<Texture>(1024, 1024, DEPTH, FLOAT, BORDER);
		ref<Texture> texBlur   = REF<Texture>(1024, 1024, ALPHA, FLOAT, BORDER);

		texDepth->Initialize(Renderer->Device);
		texShadow->Initialize(Renderer->Device);
		texBlur->Initialize(Renderer->Device);

		target = std::make_shared<RenderTarget>(1024, 1024);
		target->AddTexture(texShadow);
		target->AddTexture(texDepth);
		target->Initialize(Renderer->Device);
		
		targetBlur = std::make_shared<RenderTarget>(1024, 1024);
		targetBlur->AddTexture(texBlur);
		targetBlur->Initialize(Renderer->Device);

		// Lights

		light = DirectionalLight(directional, target, OrthographicCamera(60, 60, -50, 50));

		// Materials

		ref<Material> smat = REF<Material>();
		ref<Material> tmat = REF<Material>();

		smat->SetShader(shader);
		tmat->SetShader(shader);

		smat->Set("albedo", vector4(1, 1, 1, 1));
		tmat->Set("albedo", vector4(1, 1, 1, 1));

		smat->Set("roughness", 0.8f);
		tmat->Set("roughness", 0.8f);
		
		smat->Set("metallic", 0.2f);
		tmat->Set("metallic", 0.2f);

		smat->SetTexture("shadowMap", texShadow);
		tmat->SetTexture("shadowMap", texShadow); // shouldnt be part of material

		smat->Initialize(Renderer->Device);
		tmat->Initialize(Renderer->Device);

		// Models

		Mesh* smesh = MakeUvSphere(25, 30);
		Mesh* tmesh = MakeTetrahedron(5);

		smesh->SetMaterial(smat);
		tmesh->SetMaterial(smat);

		smesh->GenTangents();
		tmesh->GenTangents();

		smesh->Initialize(Renderer->Device);
		tmesh->Initialize(Renderer->Device);

		Model sm { smesh, 1 };
		Model tm { tmesh, 1 };

		Asset->Give<Model>("Sphere", &sm);
		Asset->Give<Model>("Tetrahedron", &tm);

		//	Player

		Mesh* pmesh = smesh->Instance();
		pmesh->Material = REF<Material>(smat->Instance());

		Model pm { pmesh, 1 };
		Asset->Give<Model>("Player", &pm);

		//	Door

		Mesh* dmesh = MakeIcosphere(0);
		dmesh->Material = REF<Material>(smat->Instance());

		dmesh->Initialize(Renderer->Device);

		Model dm { dmesh, 1 };
		Asset->Give<Model>("Door", &dm);

		// Transition mesh
		
		tranMesh = MakePlane(1, 1);
		tranMesh->Material = REF<Material>(smat->Instance());

		tranMesh->Initialize(Renderer->Device);

		// Cameras

		mainCam = new PerspectiveCamera (1.17f, 1.778f, .01f, 2000.0f);
		textCam = new OrthographicCamera(vector3::one, quaternion::from_axis_angle(vector3::unit_y, Pi), 16, 9, -10, 10);
		tranCam = new OrthographicCamera(16, 9, 0, 1);

		// Floor colliders

		Transform* tl = new Transform();
		Transform* tr = new Transform();
		Transform* tt = new Transform();
		Transform* tb = new Transform();
		Transform* to = new Transform();
		Transform* tu = new Transform();

		PlaneCollider* planel = new PlaneCollider(vector3( 1,  0,  0), -16);
		PlaneCollider* planer = new PlaneCollider(vector3(-1,  0,  0), -16);
		PlaneCollider* planet = new PlaneCollider(vector3( 0,  0,  1), -9);
		PlaneCollider* planeb = new PlaneCollider(vector3( 0,  0, -1), -9);
		PlaneCollider* planeo = new PlaneCollider(vector3( 0, -1,  0), -2);
		PlaneCollider* planeu = new PlaneCollider(vector3( 0,  1,  0),  0);

		CollisionObject* rl = new CollisionObject();
		CollisionObject* rr = new CollisionObject();
		CollisionObject* rt = new CollisionObject();
		CollisionObject* rb = new CollisionObject();
		CollisionObject* ro = new CollisionObject();
		CollisionObject* ru = new CollisionObject();

		rl->SetCol(planel);
		rr->SetCol(planer);
		rt->SetCol(planet);
		rb->SetCol(planeb);
		ro->SetCol(planeo);
		ru->SetCol(planeu);

		rl->SetTrans(tl);
		rr->SetTrans(tr);
		rt->SetTrans(tt);
		rb->SetTrans(tb);
		ro->SetTrans(to);
		ru->SetTrans(tu);

		Physics->AddCollisionObject(rl);
		Physics->AddCollisionObject(rr);
		Physics->AddCollisionObject(rt);
		Physics->AddCollisionObject(rb);
		Physics->AddCollisionObject(ro);
		Physics->AddCollisionObject(ru);

		Physics->SetGravity(vector3(0, -9.8f, 0));
		Physics->AddSolver(new ImpulseSolver());
		Physics->AddSolver(new SmoothPositionSolver());

		// Rendering pipeline

		generateShadowMap    = new GenerateShadowMap(Renderer, Space);
		postProcessShadowMap = new FilterTarget(Renderer);
		mainRender           = new Render(Renderer, Space);

		generateShadowMap   ->SetLight(light);
		postProcessShadowMap->SetIntermediate(targetBlur);
		postProcessShadowMap->SetShader(gaussian);
		mainRender          ->SetLight(light);
		mainRender          ->SetAmbiance(0.008f);
		mainRender          ->SetGamma(2.2f);

		pipeline.first(generateShadowMap)
			.then(postProcessShadowMap)
			.then(mainRender);

		// Systems

		PushSystem<PhysicsSystem>();
		PushSystem<PlayerSystem>();
		PushSystem<EnemySystem>();
		PushSystem<BulletSystem>();
		PushSystem<LevelSystem>();

		return Layer::Initialize();
	}

	void SandboxLayer::PreUpdate() {
		//if (loadLevel != "") {
		//	LoadLevel(loadLevel);
		//	loadLevel = "";
		//}
	}

	void SandboxLayer::PostUpdate() {
		//font->UpdateMesh(textMesh, std::to_string(1.0f / Time::DeltaTime()), 0.01f, 1);
		textMesh->Update(Renderer->Device);

		// Main render

		light.SetPosition(lightPos);
		light.SetRotation(quaternion::from_look_at(lightPos));

		float blurw = 1.0f / target->Width() * blurAmount;
		float blurh = 1.0f / target->Height() * blurAmount;

		generateShadowMap->SetThreshold(threshold);
		postProcessShadowMap->SetBlur(vector2(blurw, blurh));

		pipeline.execute();

		Renderer->SetCamera(textCam);
		Renderer->DrawMesh(&textTransform, textMesh);
	}
	
	void SandboxLayer::FixedUpdate() {
		Physics->Step(Time::FixedTime() * ts);
	}

	void SandboxLayer::ImGui() {
		ImGui::Begin("Sandbox");

		ImGui::SliderFloat("Time scale", &ts, 0.001f, 1);

		ImGui::SliderFloat("Ambiance", (float*)&mainRender->GetAmbiance(), 0, 1);
		ImGui::SliderFloat("Gamma", (float*)&mainRender->GetGamma(), 0, 5);

		ImGui::SliderFloat("Shadow map blur", &blurAmount, 0, 5);
		ImGui::SliderFloat("Shadow map threshold", &threshold, 0, 1);
		ImGui::SliderFloat3("Light pos", (float*)&lightPos, -5, 5);

		ImGui::SliderFloat3("Text pos",   (float*)&textTransform.Position, -8, 8);
		ImGui::SliderFloat3("Text scale", (float*)&textTransform.Scale, 0, 10);
		ImGui::SliderFloat4("Text rot",   (float*)&textTransform.Rotation, 0, 1);

		ImGui::SliderFloat("Volume", &Audio->GetVolume(), 0, 1);

		if (ImGui::Button("Start ambiance")) {
			Audio->AsStudio()->StartInstance(forestInstance);
		}

		if (ImGui::Button("Stop ambiance")) {
			Audio->AsStudio()->StopInstance(forestInstance);
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

	bool SandboxLayer::On(
		MouseMovedEvent& e)
	{
		str.clear();
		font->UpdateMesh(textMesh, std::to_string((int)e.X) + '\n' + std::to_string((int)e.Y), .01f, 1);

		return false;
	}

	bool SandboxLayer::On(
		KeyTypedEvent& e)
	{
		str += e.Character;
		font->UpdateMesh(textMesh, str, .01f, 1);

		return false;
	}

	bool SandboxLayer::On(
		ActionEvent& e)
	{
		switch (e.Action) {
			case val(Actions::SPAWN_CIRCLE_TEMP): {
				ref<Model> sphere = Asset->Load<Model>("Sphere");

				for (size_t i = 0; i < 1; i++) {
					Entity enemy = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Enemy>();
					enemy.SetComponent<Model>(*Asset->Load<Model>("Tetrahedron"));
					enemy.SetComponent<Enemy>(EnemyType::SPIN, Bullet{ LINE, 5 }, 0.2617993f, .12f, 0.0f);

					Transform* te = enemy.SetComponent<Transform>(vector3(cos(x) * 1, 15, sin(x) * 1));
					SphereCollider* se = enemy.SetComponent<SphereCollider>(vector3::zero, 1.0f);
					Rigidbody* re = enemy.SetComponent<Rigidbody>();

					re->SetMass(1);
					re->SetCol(se);
					re->SetTrans(te);

					Physics->AddRigidbody(re);
				}
				break;
			}
			case val(Actions::LOADED_LEVEL): { // hack for needing to reset space, should all be in level prefabs but thatl come later
				// Main Camera

				iw::quaternion camrot = iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
					* iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::Pi);

				iw::Entity camera = Space->CreateEntity<iw::Transform, iw::CameraController>();
				camera.SetComponent<iw::Transform>(vector3(0, 25, 0), iw::vector3::one, camrot);
				camera.SetComponent<iw::CameraController>(mainCam);
				break;
			}
		}

		return Layer::On(e);
	}
}

