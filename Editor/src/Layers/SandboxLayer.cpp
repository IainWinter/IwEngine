#include "Layers/SandboxLayer.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"
#include "Systems/BulletSystem.h"
#include "Events/ActionEvents.h"
#include "iw/graphics/Model.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Time.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Collision/PositionSolver.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/graphics/TextureAtlas.h"
#include "iw/graphics/DirectionalLight.h"
#include "iw/util/jobs/pipeline.h"
#include "imgui/imgui.h"

#include "Pipeline/GenerateShadowMap.h"
#include "Pipeline/FilterTarget.h"
#include "Pipeline/Render.h"

#include "iw/graphics/Font.h"

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

	DirectionalLight light;
	iw::vector3 lightPos = iw::vector3(2, 5, .5f);
	iw::ref<RenderTarget> target;
	iw::ref<RenderTarget> targetBlur;
	iw::ref<Shader> gaussian;
	float blurAmount = .5f;
	TextureAtlas atlasD;
	TextureAtlas atlasRG;
	TextureAtlas atlasBlur;

	iw::pipeline pipeline;
	GenerateShadowMap* generateShadowMap;
	FilterTarget* postProcessShadowMap;

	Entity camera;
	Mesh* textMesh;
	OrthographicCamera* textCam;
	iw::ref<Shader> fontShader;
	iw::ref<Font> font;

	SandboxLayer::SandboxLayer()
		: Layer("Sandbox")
	{}

	int SandboxLayer::Initialize() {
		// Font

		font = Asset->Load<Font>("fonts/twcentmt.fnt");
		textMesh = font->GenerateMesh("Winter.dev", .01f, 1);

		textMesh->Initialize(Renderer->Device);
		font->Initialize(Renderer->Device);

		fontShader = Asset->Load<Shader>("shaders/font.shader");

		Renderer->InitShader(fontShader, CAMERA);

		// Shader

		iw::ref<Shader> shader = Asset->Load<Shader>("shaders/default.shader");
		iw::ref<Shader> directional = Asset->Load<Shader>("shaders/lights/directional_transparent.shader");
		gaussian = Asset->Load<Shader>("shaders/filters/gaussian.shader");
		
		Renderer->InitShader(shader, ALL);
		Renderer->InitShader(directional, CAMERA);
		Renderer->InitShader(gaussian,    CAMERA);

		// Textures
		
		atlasD    = TextureAtlas(2048, 2048, IW::DEPTH, IW::FLOAT, IW::BORDER);
		atlasRG   = TextureAtlas(2048, 2048, IW::RG,    IW::FLOAT, IW::BORDER);
		atlasBlur = TextureAtlas(2048, 2048, IW::ALPHA, IW::FLOAT, IW::BORDER);

		atlasD   .Initialize(Renderer->Device);
		atlasRG  .Initialize(Renderer->Device);
		atlasBlur.Initialize(Renderer->Device);

		atlasD   .GenTexBounds(2, 2);
		atlasRG  .GenTexBounds(2, 2);
		atlasBlur.GenTexBounds(2, 2);

		iw::ref<Texture> subD    = atlasD   .GetSubTexture(0);
		iw::ref<Texture> subRG   = atlasRG  .GetSubTexture(0);
		iw::ref<Texture> subBlur = atlasBlur.GetSubTexture(0);

		subD   ->Initialize(Renderer->Device);
		subRG  ->Initialize(Renderer->Device);
		subBlur->Initialize(Renderer->Device);

		target = std::make_shared<RenderTarget>(1024, 1024);
		target->AddTexture(subRG);
		target->AddTexture(subD);
		target->Initialize(Renderer->Device);
		
		targetBlur = std::make_shared<RenderTarget>(1024, 1024);
		targetBlur->AddTexture(subBlur);
		targetBlur->Initialize(Renderer->Device);

		// Lights

		light = DirectionalLight(directional, target, OrthographicCamera(60, 60, -50, 50));

		// Materials

		iw::ref<Material> smat = std::make_shared<Material>();
		iw::ref<Material> tmat = std::make_shared<Material>();

		smat->SetShader(shader);
		tmat->SetShader(shader);

		smat->Set("albedo", iw::vector4(1, .95f, 1, 1));
		tmat->Set("albedo", iw::vector4(.95f, 1, 1, 1));

		smat->SetTexture("shadowMap", subRG);
		tmat->SetTexture("shadowMap", subRG);

		smat->Initialize(Renderer->Device);
		tmat->Initialize(Renderer->Device);

		// Models

		iw::ref<Model> level = Asset->Load<Model>("models/grass/grass.obj");

		for (size_t i = 0; i < level->MeshCount; i++) {
			iw::ref<Material>& mat = level->Meshes[i].Material;

			mat->SetShader(shader);

			mat->SetTexture("shadowMap", subRG);

			mat->Initialize(Renderer->Device);

			level->Meshes[i].SetTangents(0, nullptr);
			level->Meshes[i].SetBiTangents(0, nullptr);

			level->Meshes[i].Initialize(Renderer->Device);
		}

		auto a = Asset->Load<Texture>("textures/foliage/alpha_mask.jpg");
		a->Initialize(Renderer->Device);

		level->Meshes[1].Material->SetTexture("alphaMaskMap", a);
		level->Meshes[2].Material->SetTexture("alphaMaskMap", a);

		Mesh* smesh = MakeUvSphere(25, 30);
		Mesh* tmesh = MakeTetrahedron(5);

		smesh->SetMaterial(smat);
		tmesh->SetMaterial(smat);

		smesh->Initialize(Renderer->Device);
		tmesh->Initialize(Renderer->Device);

		Model sm { smesh, 1 };
		Model tm { tmesh, 1 };

		iw::ref<Model> sphere = Asset->Give<Model>("Sphere", &sm);
		iw::ref<Model> tetrahedron = Asset->Give<Model>("Tetrahedron", &tm);

		// Floor

		Entity floor = Space->CreateEntity<Transform, Model, PlaneCollider, Rigidbody>();
		floor.SetComponent<Model>(*level);

		Transform* t     = floor.SetComponent<Transform>    (iw::vector3(0, 0, 0), iw::vector3(5, 3, 5));
		PlaneCollider* s = floor.SetComponent<PlaneCollider>(iw::vector3::unit_y, 0.0f);
		Rigidbody* r     = floor.SetComponent<Rigidbody>();

		r->SetIsKinematic(false);
		r->SetMass(1);
		r->SetCol(s);
		r->SetTrans(t);
		r->SetStaticFriction(.1f);
		r->SetDynamicFriction(.01f);

		Physics->AddRigidbody(r);

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

		// Player		

		Entity player = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Player>();
		player.SetComponent<Model> (*sphere);
		player.SetComponent<Player>(-10.0f, .18f, .08f);

		Transform* tp      = player.SetComponent<Transform>     (iw::vector3(5, 1, 0));
		SphereCollider* sp = player.SetComponent<SphereCollider>(iw::vector3::zero, 1.0f);
		Rigidbody* rp      = player.SetComponent<Rigidbody>     ();

		rp->SetMass(1);
		rp->SetCol(sp);
		rp->SetTrans(tp);
		rp->SetStaticFriction(.1f);
		rp->SetDynamicFriction(.02f);

		Physics->AddRigidbody(rp);

		Entity enemy = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Enemy>();
		enemy.SetComponent<Model>(*tetrahedron);
		enemy.SetComponent<Enemy>(SPIN, 0.2617993f, .12f, 0.0f);

		Transform* te      = enemy.SetComponent<Transform>     (iw::vector3(0, 1, 0));
		SphereCollider* se = enemy.SetComponent<SphereCollider>(iw::vector3::zero, 1.0f);
		Rigidbody* re      = enemy.SetComponent<Rigidbody>     ();

		re->SetMass(1);
		re->SetCol(se);
		re->SetTrans(te);

		Physics->AddRigidbody(re);

		// Camera

		PerspectiveCamera* perspective = new PerspectiveCamera(1.17f, 1.778f, .01f, 2000.0f);

		iw::quaternion camrot = iw::quaternion::from_axis_angle(iw::vector3::unit_y, iw::PI)
			* iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::PI / 2);

		camera = Space->CreateEntity<Transform, CameraController>();
		camera.SetComponent<Transform>       (iw::vector3(0, 25, 0), iw::vector3::one, camrot);
		camera.SetComponent<CameraController>(perspective);

		textCam = new OrthographicCamera(iw::vector3::zero, camrot, 16, 9, -10, 10);

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

	float threshold = 0;

	int i = 0;

	void SandboxLayer::PostUpdate() {
		//font->UpdateMesh(textMesh, std::to_string(i++), .01f, 1);
		//textMesh->Update(Renderer->Device);

		light.SetPosition(lightPos);
		light.SetRotation(iw::quaternion::from_look_at(lightPos));

		float blurw = 1.0f / target->Width() * blurAmount;
		float blurh = 1.0f / target->Height() * blurAmount;

		generateShadowMap->SetThreshold(threshold);
		postProcessShadowMap->SetBlur(iw::vector2(blurw, blurh));

		pipeline.execute();

		//Renderer->BeginScene();

		Renderer->SetCamera(textCam);

		Renderer->SetShader(fontShader);

		fontShader->Program->GetParam("color")->SetAsFloats(&iw::vector3::one, 1, 3);
		fontShader->Program->GetParam("fontMap")->SetAsTexture(font->GetTexture(0)->Handle());

		Transform t{ iw::vector3(-7.5, 0, 3), iw::vector3::one, iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::PI / 2) };
		fontShader->Program->GetParam("model")->SetAsMat4(t.Transformation());

		Renderer->DrawMesh(&t, textMesh);

		//Renderer->EndScene();
	}

	float ts = 1.0f;
	
	void SandboxLayer::FixedUpdate() {
		Physics->Step(Time::FixedTime() * ts);
	}

	void SandboxLayer::ImGui() {
		ImGui::Begin("Sandbox");

		ImGui::SliderFloat("Time scale", &ts, 0.001f, 1);
		ImGui::SliderFloat("Shadow map blur", &blurAmount, 0, 5);
		ImGui::SliderFloat("Shadow map threshold", &threshold, 0, 1);
		ImGui::SliderFloat3("Light pos", (float*)&lightPos, -5, 5);

		ImGui::SliderFloat3("Cam pos", (float*)&camera.FindComponent<Transform>()->Position, -10, 10);

		ImGui::End();
	}

	float x = 0;
	int sc = 5;

	bool SandboxLayer::On(
		ActionEvent& e)
	{
		if (e.Action != iw::val(Actions::SPAWN_CIRCLE_TEMP))
			return false;

		iw::ref<Model> sphere = Asset->Load<Model>("Sphere");

		for (size_t i = 0; i < sc; i++) {
			Entity enemy = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Enemy>();
			enemy.SetComponent<Model>(*sphere);
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

