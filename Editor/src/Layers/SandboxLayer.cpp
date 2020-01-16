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
	iw::ref<RenderTarget> target;
	iw::ref<RenderTarget> targetBlur;
	iw::ref<Shader> gaussian;
	float blurAmount = 1.2f;
	TextureAtlas atlasD;
	TextureAtlas atlasRG;
	TextureAtlas atlasBlur;

	struct PostProcessShadowMap;

	iw::pipeline pipeline;
	PostProcessShadowMap* postProcessShadowMap;

	struct GenerateShadowMap
		: iw::node
	{
		iw::ref<Renderer> renderer;
		iw::ref<Space>    space;

		GenerateShadowMap(
			iw::ref<Renderer> r,
			iw::ref<Space> s)
			: iw::node(2, 1)
			, renderer(r)
			, space(s)
		{}

		void execute() override {
			Light* light = in<Light*>(0);
			float& threshold = in<float>(1);

			renderer->BeginLight(light);

			for (auto m_e : space->Query<Transform, Model>()) {
				auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();

				for (size_t i = 0; i < m_m->MeshCount; i++) {
					//iw::ref<Texture> t = m_m->Meshes[i].Material->GetTexture("alphaMaskMap");

					//ITexture* it = nullptr;
					//if (t) {
					//	it = t->Handle();
					//}

					//light->LightShader()->Program->GetParam("alphaMask")->SetAsTexture(it, 0);
					//light->LightShader()->Program->GetParam("alphaThreshold")->SetAsFloat(threshold);

					renderer->CastMesh(light, m_t, &m_m->Meshes[i]);
				}
			}

			renderer->EndLight(light);

			out<iw::ref<RenderTarget>>(0, light->LightTarget());
		}
	};

	struct PostProcessShadowMap
		: iw::node
	{
		iw::ref<Renderer> renderer;

		PostProcessShadowMap(
			iw::ref<Renderer> r)
			: iw::node(4, 1)
			, renderer(r)
		{}

		void execute() override {
			iw::ref<RenderTarget>& target = in<iw::ref<RenderTarget>>(0);
			iw::ref<RenderTarget>& intermediate = in<iw::ref<RenderTarget>>(1);
			iw::ref<Shader>& shader = in<iw::ref<Shader>>(2);
			iw::vector2& blur = in<iw::vector2>(3);

			renderer->SetShader(gaussian);

			shader->Program->GetParam("blurScale")->SetAsFloats(&iw::vector3(blur.x, 0, 0), 3);
			renderer->ApplyFilter(shader, target.get(), intermediate.get());

			shader->Program->GetParam("blurScale")->SetAsFloats(&iw::vector3(0, blur.y, 0), 3);
			renderer->ApplyFilter(shader, intermediate.get(), target.get());

			out<iw::ref<Texture>>(0, target->Tex(0));
		}
	};

	struct MainRender
		: iw::node
	{
		iw::ref<Renderer> renderer;
		iw::ref<Space>    space;

		MainRender(
			iw::ref<Renderer> r,
			iw::ref<Space> s)
			: iw::node(2, 0)
			, renderer(r)
			, space(s)
		{}

		void execute() override {
			iw::ref<Texture>& shadowMap = in<iw::ref<Texture>>(0);
			Light* light = in<Light*>(1);

			for (auto c_e : space->Query<Transform, CameraController>()) {
				auto [c_t, c_c] = c_e.Components.Tie<CameraComponents>();

				renderer->BeginScene(c_c->Camera);

				for (auto m_e : space->Query<Transform, Model>()) {
					auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();

					for (size_t i = 0; i < m_m->MeshCount; i++) {
						Mesh& mesh = m_m->Meshes[i];

						renderer->SetShader(mesh.Material->Shader);

						mesh.Material->Shader->Program->GetParam("lightSpace")
							->SetAsMat4(light->Cam().GetViewProjection());

						renderer->DrawMesh(m_t, &mesh);
					}
				}

				renderer->EndScene();
			}
		}
	};

	SandboxLayer::SandboxLayer()
		: Layer("Sandbox")
	{}

	int SandboxLayer::Initialize() {
		// Shader
		iw::ref<Shader> directional = Asset->Load<Shader>("shaders/lights/directional.shader");
		gaussian = Asset->Load<Shader>("shaders/filters/gaussian.shader");
		
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
		light = DirectionalLight(directional, target, OrthographicCamera(30, 30, -5, 50));
		light.SetPosition(2);
		light.SetRotation(iw::quaternion::from_look_at(iw::vector3(1)));

		iw::ref<Model> level = Asset->Load<Model>("models/grass/grass.obj");
		for (size_t i = 0; i < level->MeshCount; i++) {
			level->Meshes[i].Material->SetTexture("shadowMap", subRG);
		}

		// floor
		Entity floor = Space->CreateEntity<Transform, Model, PlaneCollider, Rigidbody>();
		Space->SetComponentData<Model>(floor, *level);

		Transform*     t = Space->SetComponentData<Transform>    (floor, iw::vector3(0, 0, 0), iw::vector3(5));
		PlaneCollider* s = Space->SetComponentData<PlaneCollider>(floor, iw::vector3::unit_y, 0.0f);
		Rigidbody*     r = Space->SetComponentData<Rigidbody>    (floor);

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
		sphere->Meshes[0].Material->SetTexture("shadowMap", subRG);

		Entity player = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Player>();
		Space->SetComponentData<Model> (player, *sphere);
		Space->SetComponentData<Player>(player, -10.0f, .18f, .08f);

		Transform* tp      = Space->SetComponentData<Transform>     (player, iw::vector3(5, 1, 0));
		SphereCollider* sp = Space->SetComponentData<SphereCollider>(player, iw::vector3::zero, 1.0f);
		Rigidbody* rp      = Space->SetComponentData<Rigidbody>     (player);

		rp->SetMass(1);
		rp->SetCol(sp);
		rp->SetTrans(tp);
		rp->SetStaticFriction(.1f);
		rp->SetDynamicFriction(.02f);
		rp->SetId(player.Index);

		Physics->AddRigidbody(rp);

		iw::ref<Model> tetrahedron = Asset->Load<Model>("Tetrahedron");
		tetrahedron->Meshes[0].Material->SetTexture("shadowMap", subRG);

		Entity enemy = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody, Enemy>();
		Space->SetComponentData<Model>(enemy, *tetrahedron);
		Space->SetComponentData<Enemy>(enemy, SPIN, 0.2617993f, .12f, 0.0f);

		Transform* te      = Space->SetComponentData<Transform>     (enemy, iw::vector3(0, 1, 0));
		SphereCollider* se = Space->SetComponentData<SphereCollider>(enemy, iw::vector3::zero, 1.0f);
		Rigidbody* re      = Space->SetComponentData<Rigidbody>     (enemy);

		re->SetMass(1);
		re->SetCol(se);
		re->SetTrans(te);
		re->SetId(enemy.Index);

		Physics->AddRigidbody(re);

		PushSystem<PlayerSystem>();
		PushSystem<EnemySystem>(sphere);
		PushSystem<BulletSystem>();

		GenerateShadowMap*     generateShadowMap    = new GenerateShadowMap(Renderer, Space);
		                       postProcessShadowMap = new PostProcessShadowMap(Renderer);
		MainRender*            mainRender           = new MainRender(Renderer, Space);

		pipeline.first(generateShadowMap)
			.then(0, 0, postProcessShadowMap)
			.then(0, 0, mainRender);

		generateShadowMap->link(postProcessShadowMap, 0, 0);

		generateShadowMap->set(0, &light);

		postProcessShadowMap->set(1, targetBlur);
		postProcessShadowMap->set(2, gaussian);

		mainRender->set(1, &light);

		//pipeline.first<GenerateShadowMap>(Renderer, Space)
		//	.then<PostProcessShadowMap>  (0, 0, Renderer)
		//	.then<MainRender>            (0, 0, Renderer, Space);

		//// get rid of first index at least

		//pipeline.init()
		//	.set(0, 0, &light)
		//	.set(1, 1, targetBlur)
		//	.set(1, 2, gaussian)
		//	.set(2, 1, &light);

		return 0;
	}

	void SandboxLayer::PostUpdate() {
		float blurw = 1.0f / (target->Width() * blurAmount);
		float blurh = 1.0f / (target->Height() * blurAmount);

		postProcessShadowMap->set(3, iw::vector2(blurw, blurh));

		pipeline.execute();

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
		ImGui::SliderFloat("Shadow map blur", &blurAmount, 0.001f, 5);

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

			Transform* te      = Space->SetComponentData<Transform>(enemy, iw::vector3(cos(x) * 1, 15, sin(x) * 1));
			SphereCollider* se = Space->SetComponentData<SphereCollider>(enemy, iw::vector3::zero, 1.0f);
			Rigidbody* re      = Space->SetComponentData<Rigidbody>(enemy);

			re->SetMass(1);
			re->SetCol(se);
			re->SetTrans(te);
			re->SetVelocity(iw::vector3(cos(x) * 0, 20, 0 * sin(x += 2 * iw::PI / sc)));
			re->SetId(enemy.Index);

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

