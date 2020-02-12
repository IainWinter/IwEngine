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

namespace IW {
	struct ModelUBO {
		iw::matrix4 model;
		iw::vector3 albedo;
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

	int SandboxLayer::Initialize() {

		// Fonts

		font = Asset->Load<Font>("fonts/arial.fnt");
		font->Initialize(Renderer->Device);

		textMesh = font->GenerateMesh("Winter.dev", .01f, 1);

		fontShader = Asset->Load<Shader>("shaders/font.shader");
		Renderer->InitShader(fontShader, CAMERA);

		iw::ref<Material> textMat = REF<Material>(fontShader);

		textMat->Set("color", iw::vector3::one);
		textMat->SetTexture("fontMap", font->GetTexture(0));

		textMesh->SetMaterial(textMat);
		textMesh->Initialize(Renderer->Device);

		textTransform = { iw::vector3(-7.5, 1, 2.5f), iw::vector3::one, iw::quaternion::identity};

		// Shader

		iw::ref<Shader> shader = Asset->Load<Shader>("shaders/default.shader");
		iw::ref<Shader> directional = Asset->Load<Shader>("shaders/lights/directional_transparent.shader");
		gaussian = Asset->Load<Shader>("shaders/filters/gaussian.shader");
		
		Renderer->InitShader(shader,      ALL);
		Renderer->InitShader(directional, CAMERA);
		Renderer->InitShader(gaussian,    CAMERA);

		// Textures
		
		Texture shadow = Texture(1024, 1024, IW::RG,    IW::FLOAT, IW::BORDER);
		iw::ref<Texture> texShadow = Asset->Give("ShadowMap", &shadow);
		
		iw::ref<Texture> texDepth  = REF<Texture>(1024, 1024, IW::DEPTH, IW::FLOAT, IW::BORDER);
		iw::ref<Texture> texBlur   = REF<Texture>(1024, 1024, IW::ALPHA, IW::FLOAT, IW::BORDER);

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

		iw::ref<Material> smat = REF<Material>();
		iw::ref<Material> tmat = REF<Material>();

		smat->SetShader(shader);
		tmat->SetShader(shader);

		smat->Set("albedo", iw::vector4(1, .95f, 1, 1));
		tmat->Set("albedo", iw::vector4(.95f, 1, 1, 1));

		smat->SetTexture("shadowMap", texShadow);
		tmat->SetTexture("shadowMap", texShadow); // shouldnt be part of material

		smat->Initialize(Renderer->Device);
		tmat->Initialize(Renderer->Device);

		// Models

		Mesh* smesh = MakeUvSphere(25, 30);
		Mesh* tmesh = MakeTetrahedron(5);

		smesh->SetMaterial(smat);
		tmesh->SetMaterial(smat);

		smesh->Initialize(Renderer->Device);
		tmesh->Initialize(Renderer->Device);

		Model sm { smesh, 1 };
		Model tm { tmesh, 1 };

		iw::ref<Model> sphere = Asset->Give<Model>("Sphere", &sm);
		Asset->Give<Model>("Tetrahedron", &tm);

		// Loading level

		LoadLevel("test.bin");

		// Floor colliders

		Transform* tl = new Transform();
		Transform* tr = new Transform();
		Transform* tt = new Transform();
		Transform* tb = new Transform();
		Transform* to = new Transform();

		PlaneCollider* planel = new PlaneCollider(iw::vector3( 1,  0,  0), -16);
		PlaneCollider* planer = new PlaneCollider(iw::vector3(-1,  0,  0), -16);
		PlaneCollider* planet = new PlaneCollider(iw::vector3( 0,  0,  1), -9);
		PlaneCollider* planeb = new PlaneCollider(iw::vector3( 0,  0, -1), -9);
		PlaneCollider* planeo = new PlaneCollider(iw::vector3( 0, -1,  0), -2);

		Rigidbody* rl = new Rigidbody(false);
		Rigidbody* rr = new Rigidbody(false);
		Rigidbody* rt = new Rigidbody(false);
		Rigidbody* rb = new Rigidbody(false);
		Rigidbody* ro = new Rigidbody(false);

		rl->SetCol(planel);
		rr->SetCol(planer);
		rt->SetCol(planet);
		rb->SetCol(planeb);
		ro->SetCol(planeo);

		rl->SetTrans(tl);
		rr->SetTrans(tr);
		rt->SetTrans(tt);
		rb->SetTrans(tb);
		ro->SetTrans(to);

		rl->SetRestitution(1);
		rr->SetRestitution(1);
		rt->SetRestitution(1);
		rb->SetRestitution(1);
		ro->SetRestitution(1);

		Physics->AddRigidbody(rl);
		Physics->AddRigidbody(rr);
		Physics->AddRigidbody(rt);
		Physics->AddRigidbody(rb);
		Physics->AddRigidbody(ro);

		Physics->SetGravity(iw::vector3(0, -9.8f, 0));
		Physics->AddDSolver(new ImpulseSolver());
		Physics->AddSolver(new PositionSolver());

		textCam = new OrthographicCamera(iw::vector3::one, iw::quaternion::from_axis_angle(iw::vector3::unit_y, iw::PI), 16, 9, -10, 10);

		// Rendering pipeline

		        generateShadowMap    = new GenerateShadowMap(Renderer, Space);
		        postProcessShadowMap = new FilterTarget(Renderer);
		Render* mainRender           = new Render(Renderer, Space);

		generateShadowMap   ->SetLight(light);
		postProcessShadowMap->SetIntermediate(targetBlur);
		postProcessShadowMap->SetShader(gaussian);
		mainRender          ->SetLight(light);

		pipeline.first(generateShadowMap)
			.then(postProcessShadowMap)
			.then(mainRender);

		// Systems

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
		
		iw::Serializer byte("test.bin");
		
		bool reset = false;
		if (reset) {
			level.Enemies.push_back(Enemy{ SPIN, 1.02f, .15f, 1.0f });
			level.Enemies.push_back(Enemy{ SPIN, 0.2617993f, .12f, 0.0f });
			level.Positions.push_back(0);
			level.Positions.push_back(1);
			level.StageName = "models/grass/grass.obj";
			byte.Write(level);
		}

		else {
			byte.Read(level);
		}

		iw::ref<Model> stage = Asset->Load<Model>(level.StageName);

		for (size_t i = 0; i < stage->MeshCount; i++) {
			iw::ref<Material>& mat = stage->Meshes[i].Material;

			mat->SetShader (Asset->Load<Shader>("shaders/default.shader"));
			mat->SetTexture("shadowMap", Asset->Load<Texture>("ShadowMap")); // shouldnt be part of material
			mat->Initialize(Renderer->Device);

			stage->Meshes[i].SetTangents(0, nullptr);
			stage->Meshes[i].SetBiTangents(0, nullptr);
			stage->Meshes[i].Initialize(Renderer->Device);
		}

		iw::ref<Texture> a = Asset->Load<Texture>("textures/foliage/alpha_mask.jpg");
		a->Initialize(Renderer->Device);

		stage->Meshes[1].Material->SetTexture("alphaMaskMap", a);
		stage->Meshes[2].Material->SetTexture("alphaMaskMap", a);

		Entity floor = Space->CreateEntity<Transform, Model, PlaneCollider, Rigidbody>();
		floor.SetComponent<Model>(*stage);

		Transform*     t = floor.SetComponent<Transform>(iw::vector3(0, 0, 0), iw::vector3(5, 3, 5));
		PlaneCollider* s = floor.SetComponent<PlaneCollider>(iw::vector3::unit_y, 0.0f);
		Rigidbody*     r = floor.FindComponent<Rigidbody>();

		r->SetIsKinematic(false);
		r->SetMass(1);
		r->SetCol(s);
		r->SetTrans(t);
		r->SetStaticFriction(.1f);
		r->SetDynamicFriction(.01f);

		Physics->AddRigidbody(r);

		for (size_t i = 0; i < level.Enemies.size(); i++) {
			Entity enemy = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Enemy>();
			enemy.SetComponent<Model>(*Asset->Load<Model>("Tetrahedron"));
			enemy.SetComponent<Enemy>(level.Enemies[i]);

			Transform*      te = enemy.SetComponent<Transform>(iw::vector3(level.Positions[i].x, 0, level.Positions[i].y));
			SphereCollider* se = enemy.SetComponent<SphereCollider>(iw::vector3::zero, 1.0f);
			Rigidbody*      re = enemy.FindComponent<Rigidbody>();

			re->SetMass(1);
			re->SetCol(se);
			re->SetTrans(te);

			Physics->AddRigidbody(re);
		}

		// Player		

		Entity player = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Player>();
		player.SetComponent<Model>(*Asset->Load<Model>("Sphere"));
		player.SetComponent<Player>(6.0f, .18f, .08f, 10);

		Transform* tp = player.SetComponent<Transform>(iw::vector3(5, 1, 0), iw::vector3(.75f));
		SphereCollider* sp = player.SetComponent<SphereCollider>(iw::vector3::zero, .75f);
		Rigidbody* rp = player.SetComponent<Rigidbody>();

		rp->SetMass(1);
		rp->SetCol(sp);
		rp->SetTrans(tp);
		rp->SetStaticFriction(.1f);
		rp->SetDynamicFriction(.02f);

		rp->SetIsLocked(iw::vector3(0, 1, 0));
		rp->SetLock(iw::vector3(0, 1, 0));

		Physics->AddRigidbody(rp);

		// Camera

		PerspectiveCamera* perspective = new PerspectiveCamera(1.17f, 1.778f, .01f, 2000.0f);

		iw::quaternion camrot = iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::PI / 2)
			* iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::PI);

		Entity camera = Space->CreateEntity<Transform, CameraController>();
		camera.SetComponent<Transform>(iw::vector3(0, 25, 0), iw::vector3::one, camrot);
		camera.SetComponent<CameraController>(perspective);
	}

	void SandboxLayer::PostUpdate() {
		textMesh->Update(Renderer->Device);

		light.SetPosition(lightPos);
		light.SetRotation(iw::quaternion::from_look_at(lightPos));

		float blurw = 1.0f / target->Width() * blurAmount;
		float blurh = 1.0f / target->Height() * blurAmount;

		generateShadowMap->SetThreshold(threshold);
		postProcessShadowMap->SetBlur(iw::vector2(blurw, blurh));

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
		ImGui::SliderFloat("Shadow map blur", &blurAmount, 0, 5);
		ImGui::SliderFloat("Shadow map threshold", &threshold, 0, 1);
		ImGui::SliderFloat3("Light pos", (float*)&lightPos, -5, 5);

		ImGui::SliderFloat3("Text pos",   (float*)&textTransform.Position, -8, 8);
		ImGui::SliderFloat3("Text scale", (float*)&textTransform.Scale, 0, 10);
		ImGui::SliderFloat4("Text rot",   (float*)&textTransform.Rotation, 0, 1);

		if (ImGui::Button("Save level")) {
			iw::Serializer out("test.bin", true);
			iw::JsonSerializer jout("test.json", true);
			
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
		if (e.Action != iw::val(Actions::SPAWN_CIRCLE_TEMP))
			return false;

		iw::ref<Model> sphere = Asset->Load<Model>("Sphere");

		for (size_t i = 0; i < sc; i++) {
			Entity enemy = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Enemy>();
			enemy.SetComponent<Model>(*Asset->Load<Model>("Tetrahedron"));
			enemy.SetComponent<Enemy>(SPIN, 0.2617993f, .12f, 0.0f);

			Transform* te      = enemy.SetComponent<Transform>     (iw::vector3(cos(x) * 1, 15, sin(x) * 1));
			SphereCollider* se = enemy.SetComponent<SphereCollider>(iw::vector3::zero, 1.0f);
			Rigidbody* re      = enemy.SetComponent<Rigidbody>();

			re->SetMass(1);
			re->SetCol(se);
			re->SetTrans(te);
			re->SetVelocity(iw::vector3(cos(x) * 0, 20, 0 * sin(x += 2 * iw::PI / sc)));

			Physics->AddRigidbody(re);
		}

		return true;
	}
}

