#include "Layers/SandboxLayer.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"
#include "Systems/BulletSystem.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Time.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PositionSolver.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Font.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/TextureAtlas.h"
#include "iw/input/Devices/Mouse.h"
#include "imgui/imgui.h"

#include "iw/reflection/serialization/JsonSerializer.h"

#include "iw/reflection/reflect/std/string.h"
#include "iw/reflection/reflect/std/vector.h"
#include "iw/reflect/math/vector2.h"
#include "iw/reflect/Components/Enemy.h"
#include "iw/reflect/Components/Level.h"

#include "iw/engine/Systems/PhysicsSystem.h"

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
	{
		//JsonSerializer json("test.json");

		//Level level;
		//json.Read(level);
	}

	int SandboxLayer::Initialize() {

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

		ref<Shader> shader = Asset->Load<Shader>("shaders/pbr.shader");
		ref<Shader> directional = Asset->Load<Shader>("shaders/lights/directional_transparent.shader");
		gaussian = Asset->Load<Shader>("shaders/filters/gaussian.shader");
		
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

		ref<Model> sphere = Asset->Give<Model>("Sphere", &sm);
		Asset->Give<Model>("Tetrahedron", &tm);

		// Cameras

		mainCam = new PerspectiveCamera (1.17f, 1.778f, .01f, 2000.0f);
		textCam = new OrthographicCamera(vector3::one, quaternion::from_axis_angle(vector3::unit_y, Pi), 16, 9, -10, 10);

		// Loading level

		LoadLevel("test.bin");

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

		Rigidbody* rl = new Rigidbody(false);
		Rigidbody* rr = new Rigidbody(false);
		Rigidbody* rt = new Rigidbody(false);
		Rigidbody* rb = new Rigidbody(false);
		Rigidbody* ro = new Rigidbody(false);
		Rigidbody* ru = new Rigidbody(false);

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

		Physics->AddRigidbody(rl);
		Physics->AddRigidbody(rr);
		Physics->AddRigidbody(rt);
		Physics->AddRigidbody(rb);
		Physics->AddRigidbody(ro);
		Physics->AddRigidbody(ru);

		Physics->SetGravity(vector3(0, -9.8f, 0));
		Physics->AddDSolver(new ImpulseSolver());
		Physics->AddSolver(new PositionSolver());

		// Rendering pipeline

		generateShadowMap    = new GenerateShadowMap(Renderer, Space);
		postProcessShadowMap = new FilterTarget(Renderer);
		mainRender           = new Render(Renderer, Space);

		generateShadowMap   ->SetLight(light);
		postProcessShadowMap->SetIntermediate(targetBlur);
		postProcessShadowMap->SetShader(gaussian);
		mainRender          ->SetLight(light);
		mainRender          ->SetAmbiance(0.1f);
		mainRender          ->SetGamma(2.2f);

		pipeline.first(generateShadowMap)
			.then(postProcessShadowMap)
			.then(mainRender);

		// Systems

		PushSystem<PhysicsSystem>();
		PushSystem<PlayerSystem>();
		PushSystem<EnemySystem>(sphere);
		PushSystem<BulletSystem>();

		return 0;
	}

	void SandboxLayer::LoadLevel(
		std::string name)
	{
		Space->Clear();

		Level level;

		Serializer byte(name);
		
		bool reset = false;
		if (reset) {
			level.Enemies.push_back(Enemy{ SPIN, 1.02f, .15f, 1.0f });
			level.Enemies.push_back(Enemy{ SPIN, 0.2617993f, .12f, 0.0f });
			level.Positions.push_back(0);
			level.Positions.push_back(1);
			level.StageName = "";
			byte.Write(level);
		}

		else {
			byte.Read(level);
		}

		// Stage

		float scale = 2.0f;
		float scaleOutX = 1.6f;
		float scaleOutY = 1.8f;

		int x = 16 * scaleOutX;
		int y = 9  * scaleOutY;

		for (int l = 0; l < 1; l++) {
			for (int i = -x; i < x; i += 4) {
				LoadTree("models/forest/tree.dae", Transform {
					vector3(i, 0, y),
					scale,
					quaternion::from_axis_angle(vector3::unit_x, Pi / 2)
					* quaternion::from_axis_angle(vector3::unit_y, rand() / (float)RAND_MAX * Pi2)
				});
			}

			for (int i = -x; i < x; i += 4) {
				LoadTree("models/forest/tree.dae", Transform {
					vector3(i, 0, -y),
					scale,
					quaternion::from_axis_angle(vector3::unit_x, Pi / 2)
					* quaternion::from_axis_angle(vector3::unit_y, rand() / (float)RAND_MAX * Pi2)
				});
			}

			for (int i = -y; i <= y; i += 3) {
				LoadTree("models/forest/tree.dae", Transform {
					vector3(x, 0, i),
					scale,
					quaternion::from_axis_angle(vector3::unit_x, Pi / 2)
					* quaternion::from_axis_angle(vector3::unit_y, rand() / (float)RAND_MAX * Pi2)
				});
			}

			for (int i = -y + 3; i < y; i += 3) {
				LoadTree("models/forest/tree.dae", Transform {
					vector3(-x, 0, i),
					scale,
					quaternion::from_axis_angle(vector3::unit_x, Pi / 2)
					* quaternion::from_axis_angle(vector3::unit_y, rand() / (float)RAND_MAX * Pi2)
				});
			}

			scaleOutX = 1.6f;
			scaleOutY = 1.8f;

			x = 16 * scaleOutX;
			y = 9  * scaleOutY;
		}

		LoadFloor("models/forest/floor.dae", Transform{
			vector3(0, 0, 0),
			vector3(32),
			quaternion::from_axis_angle(vector3::unit_x, Pi / 2)
		});

		// Enemies

		for (size_t i = 0; i < level.Enemies.size(); i++) {
			Entity enemy = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Enemy>();
			enemy.SetComponent<Model>(*Asset->Load<Model>("Tetrahedron"));
			enemy.SetComponent<Enemy>(level.Enemies[i]);

			Transform*      te = enemy.SetComponent<Transform>(vector3(level.Positions[i].x, 1, level.Positions[i].y));
			SphereCollider* se = enemy.SetComponent<SphereCollider>(vector3::zero, 1.0f);
			Rigidbody*      re = enemy.SetComponent<Rigidbody>();

			re->SetMass(1);
			re->SetCol(se);
			re->SetTrans(te);
			re->SetVelocity(-1);

			Physics->AddRigidbody(re);
		}

		// Player

		Entity player = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Player>();
		player.SetComponent<Model>(*Asset->Load<Model>("Sphere"));
		player.SetComponent<Player>(4.0f, 8 / 60.0f, .08f, 5);

		Transform*      tp = player.SetComponent<Transform>(vector3(5, 1, 0), vector3(.75f));
		SphereCollider* sp = player.SetComponent<SphereCollider>(vector3::zero, .75f);
		Rigidbody*      rp = player.SetComponent<Rigidbody>();

		rp->SetMass(1);
		rp->SetCol(sp);
		rp->SetTrans(tp);
		rp->SetStaticFriction(.1f);
		rp->SetDynamicFriction(.02f);

		rp->SetIsLocked(vector3(0, 1, 0));
		rp->SetLock(vector3(0, 1, 0));

		Physics->AddRigidbody(rp);

		// Camera

		quaternion camrot = quaternion::from_axis_angle(vector3::unit_x, Pi / 2)
			* quaternion::from_axis_angle(vector3::unit_z, Pi);

		Entity camera = Space->CreateEntity<Transform, CameraController>();
		camera.SetComponent<Transform>(vector3(0, 25, 0), vector3::one, camrot);
		camera.SetComponent<CameraController>(mainCam);
	}

	void SandboxLayer::LoadTree(
		std::string name,
		Transform transform)
	{
		ref<Model> tree = Asset->Load<Model>(name);

		if (tree->Meshes[0].VertexArray == nullptr) {
			for (size_t i = 0; i < tree->MeshCount; i++) {
				ref<Material>& mat = tree->Meshes[i].Material;

				mat->SetShader(Asset->Load<Shader>("shaders/pbr.shader"));
				mat->SetTexture("shadowMap", Asset->Load<Texture>("ShadowMap")); // shouldnt be part of material
				mat->Initialize(Renderer->Device);

				mat->Set("roughness", 0.7f);
				mat->Set("metallic", 0.0f);

				tree->Meshes[i].GenTangents();
				tree->Meshes[i].Initialize(Renderer->Device);
			}
		}

		ref<Texture> leavesAlpha = Asset->Load<Texture>("textures/forest/tree/leaves/alpha.jpg");
		tree->Meshes[1].Material->SetTexture("alphaMaskMap", leavesAlpha);
		leavesAlpha->Initialize(Renderer->Device);

		Entity ent = Space->CreateEntity<Transform, Model>();
		ent.SetComponent<Transform>(transform);
		ent.SetComponent<Model>(*tree);
	}

	void SandboxLayer::LoadFloor(
		std::string name,
		Transform transform)
	{
		ref<Model> floor = Asset->Load<Model>(name);

		if (floor->Meshes[0].VertexArray == nullptr) {
			for (size_t i = 0; i < floor->MeshCount; i++) {
				ref<Material>& mat = floor->Meshes[i].Material;

				mat->SetShader(Asset->Load<Shader>("shaders/pbr.shader"));
				mat->SetTexture("shadowMap", Asset->Load<Texture>("ShadowMap")); // shouldnt be part of material
				mat->Initialize(Renderer->Device);

				mat->Set("roughness", 0.9f);
				mat->Set("metallic", 0.1f);

				floor->Meshes[i].GenTangents();
				floor->Meshes[i].Initialize(Renderer->Device);
			}
		}

		Entity ent = Space->CreateEntity<Transform, Model>();
		ent.SetComponent<Transform>(transform);
		ent.SetComponent<Model>(*floor);
	}

	void SandboxLayer::PreUpdate() {
		if (loadLevel != "") {
			LoadLevel(loadLevel);
			loadLevel = "";
		}
	}

	void SandboxLayer::PostUpdate() {
		textMesh->Update(Renderer->Device);

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

		if (ImGui::Button("Save level")) {
			Serializer out("test.bin", true);
			JsonSerializer jout("test.json", true);
			
			Level level;
			for (auto entity : Space->Query<Transform, Enemy>()) {
				auto [transform, enemy] = entity.Components.Tie<EnemySystem::Components>();
				level.Positions.push_back({ transform->Position.x, transform->Position.z });
				level.Enemies.push_back(*enemy);
			}

			level.StageName = "models/grass/grass.obj";
			
			out.Write(level);
			jout.Write(level);
		}

		if (ImGui::Button("Load level")) {
			loadLevel = "test.bin";
		}

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
					enemy.SetComponent<Enemy>(SPIN, 0.2617993f, .12f, 0.0f);

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
			case val(Actions::RESET_LEVEL): {
				loadLevel = "test.bin";
				break;
			}
			case val(Actions::NEXT_LEVEL): {
				loadLevel = "test2.bin";
				break;
			}
		}

		return false;
	}
}

