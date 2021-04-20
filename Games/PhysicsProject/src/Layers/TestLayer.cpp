#include "Layers/TestLayer.h"
//#include "Events/ActionEvents.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include "imgui/imgui.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/EditorCameraController.h"
#include "iw/graphics/PointLight.h"
#include <iw\physics\Collision\SphereCollider.h>
#include <iw\physics\Collision\PlaneCollider.h>
#include <iw\physics\Collision\MeshCollider.h>
#include <iw\physics\Dynamics\SmoothPositionSolver.h>
#include <iw\physics\Dynamics\ImpulseSolver.h>
#include <iw\engine\Systems\PhysicsSystem.h>
#include "iw/input/Devices/Keyboard.h"
#include "iw/physics/Collision/PositionSolver.h"
#include "iw/graphics/MeshFactory.h"

#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include "iw/engine/Systems/Render/RenderSystem.h"
#include "iw/engine/Systems/Render/ShadowRenderSystem.h"
//#include "Systems/SpaceInspectorSystem.h"

#include "iw/math/vector.h"
#include "iw/math/matrix.h"

namespace iw {
	struct MeshComponents {
		Transform* Transform;
		Mesh* Mesh;
	};

	struct ModelComponents {
		Transform* Transform;
		Model* Model;
	};

	TestLayer::TestLayer()
		 : Layer("Test")
	{
		srand(time(nullptr));
	}

	int x = 0;
	Entity TestLayer::SpawnCube(vector3 s, float m, bool locked) {
		Entity entity = Space->CreateEntity<Transform, Mesh, Rigidbody>();

		Mesh* mesh;
		Collider* col;

		//if (randf() > 0.5f) {
			//col = entity.Add<MeshCollider>(MeshCollider::MakeCube());
			//mesh = entity.Set<Mesh>(cube->MakeInstance());
		//}

		//else if (randf() > 0.0f) {
		//	col  = entity.Add<MeshCollider>(MeshCollider::MakeTetrahedron());
		//	mesh = entity.Set<Mesh>(tetrahedron->MakeInstance());
		//}

		//else {
			col = entity.Add<SphereCollider>(0, 1);
			mesh = entity.Set<Mesh>(sphere->MakeInstance());

		//	s.x = s.z;
		//	s.y = s.z; // make uniform scale if where because thats how their collider works
		//}

		Transform*      trans = entity.Set<Transform>(iw::vector3(x += 5, 10, 0), s/*, iw::quaternion::from_euler_angles(randf() + 1, randf() + 1, randf() + 1)*/);
		Rigidbody*      body  = entity.Set<Rigidbody>();

		//trans->Rotation = quaternion::from_euler_angles(iw::randf() * iw::Pi2, iw::randf() * iw::Pi2, iw::randf() * iw::Pi2);
		trans->Position = vector3(iw::randf()) * 10;

		mesh->SetMaterial(REF<Material>(shader));

		mesh->Material()->Set("albedo", iw::vector4(
			randf() + 1,
			randf() + 1,
			randf() + 1,
			1.0f)
		);

		mesh->Material()->Set("reflectance", .5f * (randf() + 1));
		mesh->Material()->Set("roughness",   .5f * (randf() + 1));
		mesh->Material()->Set("metallic",    .5f * (randf() + 1));

		mesh->Material()->SetTexture("shadowMap", pointShadowTarget->Tex(0));
		//mesh->Material()->SetTexture("ww", pointShadowTarget->Tex(0));

		mesh->Material()->Initialize(Renderer->Device);
		
		body->SetTrans(trans);
		body->SetCol(col);
		body->SetIsStatic(false);
		body->Restitution = .5;
		body->DynamicFriction = .2;
		body->StaticFriction = .3;
		body->SetMass(m);

		if (locked) {
			body->IsAxisLocked = 1;
			body->AxisLock = vector3(0, 15, 0);
			body->SimGravity = false;

			body->Restitution = 0;
			body->DynamicFriction = 1;
			body->StaticFriction  =1;
		}
		else {
			//body->Velocity = vector3(25 * (iw::randf() + .5f), 25 * (iw::randf() + .5f), 25 * (iw::randf() + .5f));
		}

		Physics->AddRigidbody(body);

		return entity;
	}

	ref<Shader> dirShadowShader;

	int TestLayer::Initialize() {
		// Shaders

		                    shader    = Asset->Load<Shader>("shaders/pbr.shader");
		ref<Shader>         phong     = Asset->Load<Shader>("shaders/phong.shader");
		            dirShadowShader   = Asset->Load<Shader>("shaders/lights/directional_transparent.shader");
		ref<Shader> pointShadowShader = Asset->Load<Shader>("shaders/lights/point.shader");

		Renderer->InitShader(shader,          CAMERA | SHADOWS | LIGHTS);
		Renderer->InitShader(phong,           CAMERA | SHADOWS | LIGHTS);
		Renderer->InitShader(dirShadowShader, CAMERA);
		Renderer->InitShader(pointShadowShader);

		// Models

		MeshDescription description;

		description.DescribeBuffer(bName::POSITION,  MakeLayout<float>(3));
		description.DescribeBuffer(bName::NORMAL,    MakeLayout<float>(3));
		description.DescribeBuffer(bName::TANGENT,   MakeLayout<float>(3));
		description.DescribeBuffer(bName::BITANGENT, MakeLayout<float>(3));
		description.DescribeBuffer(bName::UV,        MakeLayout<float>(2));

		sphere      = MakeIcosphere(description, 3);
		tetrahedron = MakeTetrahedron(description, 1);
		cube        = MakeCube(description, 1);
		plane       = MakePlane(description, 1, 1);

		// Directional light shadow map textures & target
		
		ref<Texture> dirShadowColor = ref<Texture>(new Texture(2024, 2024, TEX_2D, RG,    FLOAT, BORDER));
		ref<Texture> dirShadowDepth = ref<Texture>(new Texture(2024, 2024, TEX_2D, DEPTH, FLOAT, BORDER));

		dirShadowTarget = REF<RenderTarget>();
		dirShadowTarget->AddTexture(dirShadowColor);
		dirShadowTarget->AddTexture(dirShadowDepth);

		dirShadowTarget->Initialize(Renderer->Device);

		// Point light shadow map textures & target

		ref<Texture> pointShadowDepth = ref<Texture>(new Texture(1024, 1024, TEX_CUBE, DEPTH, FLOAT, EDGE));
		
		pointShadowTarget = REF<RenderTarget>(true);
		pointShadowTarget->AddTexture(pointShadowDepth);

		pointShadowTarget->Initialize(Renderer->Device);

		// Floor

		{
			Ground = Space->CreateEntity<Transform, Mesh, PlaneCollider, CollisionObject>();

			Transform*       trans = Ground.Set<Transform>(iw::vector3(0, -2, 0), vector3(22));
			Mesh*            mesh  = Ground.Set<Mesh>(plane->MakeInstance());
			PlaneCollider*   col   = Ground.Set<PlaneCollider>(vector3::unit_y, 0);
			CollisionObject* obj   = Ground.Set<CollisionObject>();

			mesh->SetMaterial(REF<Material>(shader));

			mesh->Material()->Set("albedo", iw::Color(0.002f, 0.144f, 0.253f));

			mesh->Material()->Set("reflectance", 0.06f);
			mesh->Material()->Set("roughness",   0.8f);
			mesh->Material()->Set("metallic",    0.8f);
			
			mesh->Material()->SetTexture("shadowMap",  dirShadowTarget->Tex(0));

			mesh->Material()->Initialize(Renderer->Device);

			obj->SetTrans(trans);
			obj->SetCol(col);

			Physics->AddCollisionObject(obj);
		}

		{
			PlaneCollider*   col1 = new PlaneCollider(vector3( 0, 0,  1), -20);
			PlaneCollider*   col2 = new PlaneCollider(vector3( 0, 0, -1), -20);
			PlaneCollider*   col3 = new PlaneCollider(vector3( 1, 0,  0), -20);
			PlaneCollider*   col4 = new PlaneCollider(vector3(-1, 0,  0), -20);

			CollisionObject* obj1 = new CollisionObject();
			CollisionObject* obj2 = new CollisionObject();
			CollisionObject* obj3 = new CollisionObject();
			CollisionObject* obj4 = new CollisionObject();

			obj1->SetCol(col1);
			obj2->SetCol(col2);
			obj3->SetCol(col3);
			obj4->SetCol(col4);

			Physics->AddCollisionObject(obj1);
			Physics->AddCollisionObject(obj2);
			Physics->AddCollisionObject(obj3);
			Physics->AddCollisionObject(obj4);
		}

		DirectionalLight* dirLight = new DirectionalLight(10, OrthographicCamera(60, 60, -100, 100));
		dirLight->SetRotation(quaternion(0.872f, 0.0f, 0.303f, 0.384f));
		dirLight->SetShadowShader(dirShadowShader);
		dirLight->SetShadowTarget(dirShadowTarget);

		// scene

		MainScene->AddLight(dirLight);

		//delete sphere;

		Physics->SetGravity(vector3(0, -9.81f, 0));
		//Physics->AddSolver(new ImpulseSolver());
		//Physics->AddSolver(new SmoothPositionSolver());

		EditorCameraControllerSystem* system = PushSystem<EditorCameraControllerSystem>();

		MainScene->SetMainCamera(system->GetCamera());
		
		PushSystem<PhysicsSystem>();
		PushSystem<iw::ShadowRenderSystem>(MainScene);
		PushSystem<iw::RenderSystem>(MainScene);

		Time::SetFixedTime(0.05f);

		srand(19);

		Ball = SpawnCube(2);

		return Layer::Initialize();
	}

	void TestLayer::FixedUpdate() {

		matrix<6, 3> mat({
			1, 2, 3, 1, 0, 0,
			4, 5, 6, 0, 1, 0,
			7, 2, 9, 0, 0, 1
		});

		// Fills in identity

		for (size_t r = 0; r < mat.rows; r++)
		for (size_t c = 0; c < mat.cols; c++) {
			if (c == r + mat.rows) {
				mat.columns[c][r] = 1;
			}
		}

		/************** partial pivoting **************/

		for (size_t r = mat.rows; r > 0; r--) {
			if (mat.columns[0][r - 1] >= mat.columns[0][r]) continue;

			for (size_t c = 0; c < mat.cols; c++) {
				float t = mat.columns[c][r];
				mat.columns[c][r]     = mat.columns[c][r - 1];
				mat.columns[c][r - 1] = t;
			}
		}

		/********** reducing to diagonal  matrix ***********/

		for (size_t r = 0; r < mat.rows; r++)
		for (size_t c = 0; c < mat.rows; c++) {
			if (c == r) continue;

			float d = mat.columns[r][c] / mat.columns[r][r];

			for (size_t k = 0; k < mat.cols; k++) {
				mat.columns[k][c] -= mat.columns[k][r] * d;
			}
		}

		/************** reducing to unit matrix *************/

		for (size_t r = 0; r < mat.rows; r++) {
			float d = mat.columns[r][r];

			for (size_t c = 0; c < mat.cols; c++) {
				mat.columns[c][r] = mat.columns[c][r] / d;
			}
		}

		//matrix<4, 4> mat({
		//	1, 3, 5, 9,
		//	1, 3, 1, 7, 
		//	4, 3, 9, 7, 
		//	5, 2, 0, 9
		//});

		//float det = mat.determinant();



		float beta = .1;
		float dt = iw::FixedTime();
		
		float& y  = Ball.Find<iw::Transform>()->Position.y;
		float& vy = Ball.Find<iw::Rigidbody>()->Velocity.y;

		vy += Physics->Gravity().y;

		if (y <= 0) {
			vy = -beta / dt * y;
		}
	}

	float thresh = .5f;

	void TestLayer::ImGui() {
		ImGui::Begin("Test");

		for (PointLight* light : MainScene->PointLights()) {
			vector3 pos = light->Position();
			ImGui::SliderFloat3("Light pos", (float*)&pos, -25, 25);
			light->SetPosition(pos);

			float rad = light->Radius();
			ImGui::SliderFloat("Light rad", (float*)&rad, 0, 100);
			light->SetRadius(rad);
		}

		for (DirectionalLight* light : MainScene->DirectionalLights()) {
			quaternion rot = light->Rotation();
			ImGui::SliderFloat4("Light rot", (float*)&rot, 0, 1);
			light->SetRotation(rot.normalized());
		}

		if (ImGui::Button("Spawn Cube")) {
			srand(3);
			for (int i = 0; i < 50; i++) {
				SpawnCube(vector3(1.5f * (iw::randf() + 1.25f), 1.5f * (iw::randf() + 1.25f), 1.5f * (iw::randf() + 1.25f)));
			}
		}

		if (ImGui::Button("Shuffle properties")) {
			for (auto entity : Space->Query<Transform, Mesh>()) {
				auto [transform, mesh] = entity.Components.Tie<MeshComponents>();

				mesh->Material()->Set("albedo", iw::vector4(
					rand() / (float)RAND_MAX,
					rand() / (float)RAND_MAX,
					rand() / (float)RAND_MAX,
					1.0f)
				);

				mesh->Material()->Set("reflectance", rand() / (float)RAND_MAX);
				mesh->Material()->Set("roughness",   rand() / (float)RAND_MAX);
				mesh->Material()->Set("metallic",    rand() / (float)RAND_MAX);
			}
		}

		ImGui::SliderFloat("alpha", &thresh, 0, 1);

		dirShadowShader->Use(Renderer->Device);
		dirShadowShader->Handle()->GetParam("alphaThreshold")->SetAsFloat(thresh);


		//for (auto entity : Space->Query<Transform, Mesh>()) {
		//	auto [transform, mesh] = entity.Components.Tie<MeshComponents>();

		//	std::stringstream ss;
		//	ss << "Reflectance " << entity.Index;

		//	ImGui::SliderFloat(ss.str().c_str(), mesh->Material->Get<float>("reflectance"), 0, 1);
		//	
		//	ss = std::stringstream();
		//	ss << "Roughness " << entity.Index;
		//	
		//	ImGui::SliderFloat(ss.str().c_str(), mesh->Material->Get<float>("roughness"), 0, 1);

		//	ss = std::stringstream();
		//	ss << "Metallic " << entity.Index;

		//	ImGui::SliderFloat(ss.str().c_str(), mesh->Material->Get<float>("metallic"), 0, 1);
		//}

		ImGui::End();
	}
}

