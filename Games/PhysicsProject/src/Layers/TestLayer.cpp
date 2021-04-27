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

#include "glm/gtc/random.hpp"

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
	Entity TestLayer::SpawnCube(glm::vec3 s, float m, bool locked) {
		Entity entity = Space->CreateEntity<Transform, Mesh, Rigidbody>();

		Mesh* mesh;
		Collider* col;

		if (x > 0) {
			col = entity.Add<MeshCollider>(MeshCollider::MakeCube());
			mesh = entity.Set<Mesh>(cube->MakeInstance());
		}

		//else if (randf() > 0.0f) {
		//	col  = entity.Add<MeshCollider>(MeshCollider::MakeTetrahedron());
		//	mesh = entity.Set<Mesh>(tetrahedron->MakeInstance());
		//}

		else {
			col = entity.Add<SphereCollider>(glm::vec3(0), 1);
			mesh = entity.Set<Mesh>(sphere->MakeInstance());

			s.x = s.z;
			s.y = s.z; // make uniform scale if where because thats how their collider works
		}

		Transform*      trans = entity.Set<Transform>(glm::vec3(x += 5, 10, 0), s/*, iw::quaternion::from_euler_angles(randf() + 1, randf() + 1, randf() + 1)*/);
		Rigidbody*      body  = entity.Set<Rigidbody>();

		//trans->Rotation = quaternion::from_euler_angles(iw::randf() * iw::Pi2, iw::randf() * iw::Pi2, iw::randf() * iw::Pi2);
		trans->Position = glm::ballRand(10.f);

		mesh->SetMaterial(REF<Material>(shader));

		mesh->Material()->Set("albedo", glm::vec4(
			glm::ballRand(1.f),
			1.0f)
		);

		mesh->Material()->Set("reflectance", .5f * glm::linearRand(0.f, 1.f));
		mesh->Material()->Set("roughness",   .5f * glm::linearRand(0.f, 1.f));
		mesh->Material()->Set("metallic",    .5f * glm::linearRand(0.f, 1.f));

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
			body->IsAxisLocked = glm::vec3();
			body->AxisLock = glm::vec3(0, 15, 0);
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

		sphere      = MakeIcosphere(description, 4);
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

			Transform*       trans = Ground.Set<Transform>(glm::vec3(0, -2, 0), glm::vec3(22));
			Mesh*            mesh  = Ground.Set<Mesh>(plane->MakeInstance());
			PlaneCollider*   col   = Ground.Set<PlaneCollider>(glm::vec3(0, 1, 0), 0);
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
			PlaneCollider*   col1 = new PlaneCollider(glm::vec3( 0, 0,  1), -20);
			PlaneCollider*   col2 = new PlaneCollider(glm::vec3( 0, 0, -1), -20);
			PlaneCollider*   col3 = new PlaneCollider(glm::vec3( 1, 0,  0), -20);
			PlaneCollider*   col4 = new PlaneCollider(glm::vec3(-1, 0,  0), -20);

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
		dirLight->SetRotation(glm::quat(0.872f, 0.0f, 0.303f, 0.384f));
		dirLight->SetShadowShader(dirShadowShader);
		dirLight->SetShadowTarget(dirShadowTarget);

		// scene

		MainScene->AddLight(dirLight);

		//delete sphere;

		//Physics->SetGravity(vector3(0, -9.81f, 0));
		//Physics->AddSolver(new ImpulseSolver());
		//Physics->AddSolver(new SmoothPositionSolver());

		EditorCameraControllerSystem* system = PushSystem<EditorCameraControllerSystem>();

		MainScene->SetMainCamera(system->GetCamera());
		
		PushSystem<PhysicsSystem>();
		PushSystem<iw::ShadowRenderSystem>(MainScene);
		PushSystem<iw::RenderSystem>(MainScene);

		Time::SetFixedTime(0.05f);

		srand(19);

		Ball = SpawnCube(glm::vec3(2));

		Ball.Find<iw::Rigidbody>()->SimGravity = false;

		Box = SpawnCube(glm::vec3(2));

		TestDebug = SpawnCube(glm::vec3(.1));
		TestDebug.Find<iw::Rigidbody>()->IsKinematic = false;

		return Layer::Initialize();
	}

	void TestLayer::FixedUpdate() { // https://blog.gtiwari333.com/2009/12/c-c-code-gauss-jordan-method-for.html
		// Position ball

		glm::vec3& targetPos = Ball.Find<Rigidbody>()->Trans().Position;
		targetPos.y = 10;
		//pos.x = cos(iw::TotalTime()) * 10;
		//pos.z = sin(iw::TotalTime()) * 10;

		// Move box twoards ball

		//vector3 r = 1; auto [x, y, z] = r;

		//float beta = 1;
		//float dt = iw::FixedTime();
		//
		//Transform* transform = Box.Find<iw::Transform>();
		//Rigidbody* rigidbody = Box.Find<iw::Rigidbody>();

		//rigidbody->Trans().Rotation *= quaternion(iw::FixedTime(), iw::FixedTime(), iw::FixedTime(), 1);

		//vector3 cornerPos = transform->Position + r * transform->Scale * matrix3::create_from_quaternion(transform->Rotation).inverted();

		//TestDebug.Find<iw::Rigidbody>()->Trans().Position = cornerPos;

		//matrix<3, 3> invMass = matrix<3, 3>(rigidbody->InvMass);
		//matrix<3, 3> invInrt = inverted(matrix<3, 3>());

		//vector3& c = cornerPos - targetPos;
		//vector3& v = rigidbody->Velocity + rigidbody->AngularVelocity.cross(r);

		//auto [cx, cy, cz] = -(v + (beta / dt) * c);
		//vector<3> C = vector<3>{ cx, cy, cz };

		//matrix<3, 3> s = Skew(vector<3>{-x, -y, -z});
		//matrix<3, 3> k = invMass + s * invInrt * transposed(s);
		//vector<3> l = inverted(k) * C;

		//vector<3> dv = invMass * l;
		//vector<3> da = (invInrt * transposed(s)) * l;

		//auto [dvx, dvy, dvz] = dv.components;
		//auto [dax, day, daz] = da.components;

		//rigidbody->Velocity += vector3(dvx, dvy, dvz);
		//rigidbody->Velocity *= .98f;

		//rigidbody->AngularVelocity += vector3(dax, day, daz);
		//rigidbody->AngularVelocity *= .98f;
	}

	float thresh = .5f;

	void TestLayer::ImGui() {
		ImGui::Begin("Test");

		for (PointLight* light : MainScene->PointLights()) {
			glm::vec3 pos = light->Position();
			ImGui::SliderFloat3("Light pos", (float*)&pos, -25, 25);
			light->SetPosition(pos);

			float rad = light->Radius();
			ImGui::SliderFloat("Light rad", (float*)&rad, 0, 100);
			light->SetRadius(rad);
		}

		for (DirectionalLight* light : MainScene->DirectionalLights()) {
			glm::quat rot = light->Rotation();
			ImGui::SliderFloat4("Light rot", (float*)&rot, 0, 1);
			light->SetRotation(glm::normalize(rot));
		}

		if (ImGui::Button("Spawn Cube")) {
			srand(3);
			for (int i = 0; i < 50; i++) {
				SpawnCube(glm::ballRand(1.5f) + 1.25f);
			}
		}

		if (ImGui::Button("Shuffle properties")) {
			for (auto entity : Space->Query<Transform, Mesh>()) {
				auto [transform, mesh] = entity.Components.Tie<MeshComponents>();

				mesh->Material()->Set("albedo", glm::vec4(
					glm::ballRand(1.f),
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

