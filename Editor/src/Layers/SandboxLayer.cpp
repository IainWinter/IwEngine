#include "Layers/SandboxLayer.h"
#include "iw/graphics/Model.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Time.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "Events/ActionEvents.h"
#include "imgui/imgui.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Collision/PositionSolver.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/graphics/TextureAtlas.h"
#include "iw/graphics/DirectionalLight.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"
#include "Systems/BulletSystem.h"

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

	DirectionalLight light;
	iw::ref<RenderTarget> target;
	iw::ref<RenderTarget> targetBlur;
	iw::ref<Shader> gaussian;
	iw::ref<Shader> nul;
	float blurAmount = 1.2f;

	int SandboxLayer::Initialize() {
		// Shader
		iw::ref<Shader> directional = Asset->Load<Shader>("shaders/lights/directional.shader");
		gaussian = Asset->Load<Shader>("shaders/filters/gaussian.shader");
		nul = Asset->Load<Shader>("shaders/filters/null.shader");
		
		Renderer->InitShader(directional, CAMERA);
		Renderer->InitShader(gaussian, CAMERA);
		Renderer->InitShader(nul, CAMERA);

		// Textures
		
		TextureAtlas atlasD = TextureAtlas(2048, 2048, IW::DEPTH, IW::FLOAT);
		atlasD.Initialize(Renderer->Device);
		atlasD.GenTexBounds(2, 2);

		TextureAtlas atlasRG = TextureAtlas(2048, 2048, IW::RG, IW::FLOAT);
		atlasRG.Initialize(Renderer->Device);
		atlasRG.GenTexBounds(2, 2);

		TextureAtlas atlasBlur = TextureAtlas(2048, 2048, IW::ALPHA, IW::FLOAT);
		atlasBlur.Initialize(Renderer->Device);
		atlasBlur.GenTexBounds(2, 2);

		iw::ref<Texture> subD    = atlasD .GetSubTexture(0);
		iw::ref<Texture> subRG   = atlasRG.GetSubTexture(0);
		iw::ref<Texture> subBlur = atlasBlur.GetSubTexture(0);

		subD   ->Initialize(Renderer->Device);
		subRG  ->Initialize(Renderer->Device);
		subBlur->Initialize(Renderer->Device);

		// Target
		//iw::ref<RenderTarget> target = Renderer->BuildRenderTarget()
		//	.SetWidth(1024)
		//	.SetHeight(1024)
		//	.AddTexture(sub)
		//	.Initialize();

		target = std::make_shared<RenderTarget>(1024, 1024);
		target->AddTexture(subRG);
		target->AddTexture(subD);
		target->Initialize(Renderer->Device);
		
		targetBlur = std::make_shared<RenderTarget>(1024, 1024);
		targetBlur->AddTexture(subBlur);
		targetBlur->Initialize(Renderer->Device);

		// Light
		light = DirectionalLight(directional, target, OrthographicCamera(40, 40, -50, 50));
		light.SetPosition(10);
		light.SetRotation(iw::quaternion::from_look_at(iw::vector3(1, 2, 1)));

		iw::ref<Model> plane = Asset->Load<Model>("Floor");
		plane->Meshes[0].Material->SetTexture("shadowMap", subRG);

		// floor
		Entity floor = Space->CreateEntity<Transform, Model, PlaneCollider, Rigidbody>();
		Space->SetComponentData<Model>(floor, *plane);

		Transform*     t = Space->SetComponentData<Transform>(floor, iw::vector3(0, 0, 0), iw::vector3(15, 1, 15));
		PlaneCollider* s = Space->SetComponentData<PlaneCollider>(floor, iw::vector3::unit_y, 0.0f);
		Rigidbody*     r = Space->SetComponentData<Rigidbody>(floor);

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

		PlaneCollider* planel = new PlaneCollider(iw::vector3( 1, 0,  0), -10);
		PlaneCollider* planer = new PlaneCollider(iw::vector3(-1, 0,  0), -10);
		PlaneCollider* planet = new PlaneCollider(iw::vector3( 0, 0,  1), -10);
		PlaneCollider* planeb = new PlaneCollider(iw::vector3( 0, 0, -1), -10);
		PlaneCollider* planeo = new PlaneCollider(iw::vector3( 0, -1, 0), -2);

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

		Physics->AddRigidbody(rl);
		Physics->AddRigidbody(rr);
		Physics->AddRigidbody(rt);
		Physics->AddRigidbody(rb);
		Physics->AddRigidbody(ro);

		Physics->SetGravity(iw::vector3(0, -9.8f, 0));
		Physics->AddDSolver(new ImpulseSolver());
		Physics->AddSolver(new PositionSolver());

		// Player

		iw::ref<Model> sphere = Asset->Load<Model>("Sphere");

		Entity player = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Player>();
		Space->SetComponentData<Model>(player, *sphere);
		Space->SetComponentData<Player>(player, -10.0f, .18f, .08f);

		Transform* tp      = Space->SetComponentData<Transform>     (player, iw::vector3(0, 10, 0));
		SphereCollider* sp = Space->SetComponentData<SphereCollider>(player, iw::vector3::zero, 1.0f);
		Rigidbody* rp      = Space->SetComponentData<Rigidbody>     (player);

		rp->SetMass(1);
		rp->SetCol(sp);
		rp->SetTrans(tp);
		rp->SetStaticFriction(.1f);
		rp->SetDynamicFriction(.02f);

		Physics->AddRigidbody(rp);

		iw::ref<Model> tetrahedron = Asset->Load<Model>("Tetrahedron");

		Entity enemy = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Enemy>();
		Space->SetComponentData<Model>(enemy, *tetrahedron);
		Space->SetComponentData<Enemy>(enemy, SPIN, 0.2617993f, .12f, 0.0f);

		Transform* te      = Space->SetComponentData<Transform>     (enemy, iw::vector3(0, 1, 0));
		SphereCollider* se = Space->SetComponentData<SphereCollider>(enemy, iw::vector3::zero, 1.0f);
		Rigidbody* re      = Space->SetComponentData<Rigidbody>     (enemy);

		re->SetMass(1);
		re->SetCol(se);
		re->SetTrans(te);

		Physics->AddRigidbody(re);

		PushSystem<PlayerSystem>();
		PushSystem<EnemySystem>(sphere);
		PushSystem<BulletSystem>();

		return 0;
	}

	void SandboxLayer::PostUpdate() {
		//Renderer->BuildSceneData("Main")
		//	.SetWorldOrigin(iw::matrix4::identity)
		//	.SetCamera(camera)
		//	.AddLight(light)
		//	.Build();

		//Renderer->BuildSceneData()
		//	.SetCamera()
		//	.SetTarget();

		//Renderer->BeginLight();

		//Renderer->CastMesh();

		//Renderer->EndLight(); 

		Renderer->BeginLight(&light);

		for (auto m_e : Space->Query<Transform, Model>()) {
			auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();

			for (size_t i = 0; i < m_m->MeshCount; i++) {
				Renderer->CastMesh(&light, m_t, &m_m->Meshes[i]);
			}
		}

		Renderer->EndLight(&light);

		float blurw = 1.0f / (target->Width()  * blurAmount);
		float blurh = 1.0f / (target->Height() * blurAmount);

		Renderer->SetShader(gaussian);

		gaussian->Program->GetParam("blurScale")->SetAsFloats(&iw::vector3(blurw, 0, 0), 3);
		Renderer->ApplyFilter(gaussian, target.get(), targetBlur.get());

		gaussian->Program->GetParam("blurScale")->SetAsFloats(&iw::vector3(0, blurh, 0), 3);
		Renderer->ApplyFilter(gaussian, targetBlur.get(), target.get());

		for (auto c_e : Space->Query<Transform, CameraController>()) {
			auto [c_t, c_c] = c_e.Components.Tie<CameraComponents>();

			Renderer->BeginScene(c_c->Camera);

			for (auto m_e : Space->Query<Transform, Model>()) {
				auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();

				for (size_t i = 0; i < m_m->MeshCount; i++) {
					Mesh& mesh = m_m->Meshes[i];

					Renderer->SetShader(mesh.Material->Shader);

					mesh.Material->Shader->Program->GetParam("lightSpace")
						->SetAsMat4(light.Cam().GetViewProjection());

					Renderer->DrawMesh(m_t, &mesh);
				}
			}

			Renderer->EndScene();
		}

		for (auto p_e : Space->Query<Transform, PlaneCollider>()) {
			auto [p_t, p_p] = p_e.Components.Tie<PlaneComponents>();

			if (IW::Mouse::ButtonDown(IW::XMOUSE1)) {
				p_t->Rotation *= iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::PI * Time::DeltaTime());
			}

			if (IW::Mouse::ButtonDown(IW::XMOUSE2)) {
				p_t->Rotation *= iw::quaternion::from_axis_angle(iw::vector3::unit_x, -iw::PI * Time::DeltaTime());
			}
		}
	}

	float ts = 0.1f;
	
	void SandboxLayer::FixedUpdate() {
		Physics->Step(Time::FixedTime() * ts);
	}

	void SandboxLayer::ImGui() {
		ImGui::Begin("Sandbox");

		ImGui::SliderFloat("Time scale", &ts, 0.001f, 1);
		ImGui::SliderFloat("Shadow map blur", &blurAmount, 0, 5);

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
			Space->SetComponentData<Model>(enemy, *sphere);
			Space->SetComponentData<Enemy>(enemy, SPIN, 0.2617993f, .12f, 0.0f);

			Transform* te = Space->SetComponentData<Transform>(enemy, iw::vector3(cos(x) * 1, 15, sin(x) * 1));
			SphereCollider* se = Space->SetComponentData<SphereCollider>(enemy, iw::vector3::zero, 1.0f);
			Rigidbody* re = Space->SetComponentData<Rigidbody>(enemy);

			re->SetMass(1);
			re->SetCol(se);
			re->SetTrans(te);
			re->SetVelocity(iw::vector3(cos(x) * 0, 20, 0 * sin(x += 2 * iw::PI / sc)));

			Physics->AddRigidbody(re);

			//Entity ent = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody>();

			//Space->SetComponentData<Model>(ent, *sphere);

			//Transform*      t = Space->SetComponentData<Transform>     (ent, iw::vector3(cos(x) * 0, 15, sin(x) * 0));
			//SphereCollider* s = Space->SetComponentData<SphereCollider>(ent, iw::vector3::zero, 1.0f);
			//Rigidbody*      r = Space->SetComponentData<Rigidbody>     (ent);

			//r->SetMass(1);
			////r->ApplyForce(iw::vector3(cos(x += .1f) * 50, 500, sin(x / .1f) * 50));
			//r->SetVelocity(iw::vector3(cos(x) * 0, 20, 0 * sin(x += 2 * iw::PI / sc)));
			//r->SetCol(s);
			//r->SetTrans(t);
			//r->SetStaticFriction(.1f);
			//r->SetDynamicFriction(.02f);

			//Physics->AddRigidbody(r);
		}

		return true;
	}
}

