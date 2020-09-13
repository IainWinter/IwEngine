#include "Layers/TestLayer.h"
#include "Events/ActionEvents.h"
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

#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include "iw/engine/Systems/Render/MeshRenderSystem.h"
#include "iw/engine/Systems/Render/MeshShadowRenderSystem.h"
#include "Systems/SpaceInspectorSystem.h"

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

	float xOff = 0;

	void TestLayer::SpawnCube(vector3 s) {
		Entity entity = Space->CreateEntity<Transform, Mesh, MeshCollider, Rigidbody>();

		Transform*      trans = entity.Set<Transform>(iw::vector3(xOff, 0, 0), s);
		Mesh*           mesh  = entity.Set<Mesh>(sphere->MakeInstance());
		MeshCollider*   col   = entity.Set<MeshCollider>(MeshCollider::MakeCube());
		Rigidbody*      body  = entity.Set<Rigidbody>();

		xOff += 10;

		mesh->SetMaterial(REF<Material>(shader));

		mesh->Material()->Set("albedo", iw::vector4(
			.1,//rand() / (float)RAND_MAX,
			rand() / (float)RAND_MAX,
			rand() / (float)RAND_MAX,
			1.0f)
		);

		mesh->Material()->Set("reflectance", rand() / (float)RAND_MAX);
		mesh->Material()->Set("roughness", rand() / (float)RAND_MAX);
		mesh->Material()->Set("metallic", rand() / (float)RAND_MAX);

		mesh->Material()->SetTexture("shadowMap", pointShadowTarget->Tex(0));
		//mesh->Material()->SetTexture("ww", pointShadowTarget->Tex(0));

		mesh->Material()->Initialize(Renderer->Device);

		body->SetTrans(trans);
		body->SetCol(col);

		body->SetOnCollision([&](auto manifold, auto dt) {
			ref<Material> mat1 = Space->FindEntity<Rigidbody>(manifold.ObjA).Find<Mesh>()->Material();
			ref<Material> mat2 = Space->FindEntity<Rigidbody>(manifold.ObjB).Find<Mesh>()->Material();

			mat1->Set("albedo", iw::vector4(1, 0.25, 0.25, 1));
			mat2->Set("albedo", iw::vector4(1, 0.25, 0.25, 1));
		});

		body->SetDynamicFriction(0.1f);

		Physics->AddRigidbody(body);
	}

	ref<Shader> dirShadowShader;

	int TestLayer::Initialize() {
		// Shaders

		                    shader    = Asset->Load<Shader>("shaders/pbr.shader");
		ref<Shader>         phong     = Asset->Load<Shader>("shaders/phong.shader");
		            dirShadowShader   = Asset->Load<Shader>("shaders/lights/directional_transparent.shader");
		ref<Shader> pointShadowShader = Asset->Load<Shader>("shaders/lights/point.shader");

		Renderer->InitShader(shader,          CAMERA | SHADOWS | LIGHTS);
		Renderer->InitShader(phong,          CAMERA | SHADOWS | LIGHTS);
		Renderer->InitShader(dirShadowShader, CAMERA);
		Renderer->InitShader(pointShadowShader);

		// Models

		MeshDescription description;

		description.DescribeBuffer(bName::POSITION,  MakeLayout<float>(3));
		description.DescribeBuffer(bName::NORMAL,    MakeLayout<float>(3));
		description.DescribeBuffer(bName::TANGENT,   MakeLayout<float>(3));
		description.DescribeBuffer(bName::BITANGENT, MakeLayout<float>(3));
		description.DescribeBuffer(bName::UV,        MakeLayout<float>(2));

		sphere = &*Asset->Load<iw::Model>("models/cube.gltf")->GetMesh(0).Data();
		plane  = MakePlane    (description, 1, 1);

		//sphere->GenTangents();
		//plane ->GenTangents();

		sphere->Initialize(Renderer->Device);
		plane ->Initialize(Renderer->Device);

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
			Entity entity = Space->CreateEntity<Transform, Mesh, PlaneCollider, CollisionObject>();

			Transform*       trans = entity.Set<Transform>(iw::vector3(0, 0, 0), vector3(22));
			Mesh*            mesh  = entity.Set<Mesh>(plane->MakeInstance());
			PlaneCollider*   col   = entity.Set<PlaneCollider>(vector3::unit_y, 0);
			CollisionObject* obj   = entity.Set<CollisionObject>();

			mesh->SetMaterial(REF<Material>(shader));

			mesh->Material()->Set("albedo", iw::vector4(1.0f));

			mesh->Material()->Set("reflectance", rand() / (float)RAND_MAX);
			mesh->Material()->Set("roughness",   rand() / (float)RAND_MAX);
			mesh->Material()->Set("metallic",    rand() / (float)RAND_MAX);
			
			mesh->Material()->SetTexture("shadowMap",  dirShadowTarget  ->Tex(0));
			//mesh->Material()->SetTexture("shadowMap2", pointShadowTarget->Tex(0));

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

		// Camera controller
		
		//Camera* camera = new PerspectiveCamera(1.17f, 1.77f, .01f, 1000.0f);

		//{
		//	Entity entity = Space->CreateEntity<Transform, EditorCameraController>();

		//	Transform* transform = entity.Set<Transform>(vector3(0, 6, -5));
		//	                       entity.Set<EditorCameraController>(camera);

		//	camera->SetTrans(transform);
		//}

		// lights

		//PointLight* light = new PointLight(12.0f, 1.0f);
		//light->SetPosition(iw::vector3(-4.5f, 8.0f, 10.0f));
		//light->SetShadowShader(pointShadowShader);
		//light->SetShadowTarget(pointShadowTarget);

		//PointLight* light2 = new PointLight(12.0f, 1.0f);
		//light2->SetPosition(iw::vector3( 4.5f, 8.0f, 10.0f));

		DirectionalLight* dirLight = new DirectionalLight(10, OrthographicCamera(60, 60, -100, 100));
		dirLight->SetRotation(quaternion(0.872f, 0.0f, 0.303f, 0.384f));
		dirLight->SetShadowShader(dirShadowShader);
		dirLight->SetShadowTarget(dirShadowTarget);

		// scene

		//MainScene->AddLight(light);
		MainScene->AddLight(dirLight);

		//delete sphere;

		//Physics->SetGravity(vector3(0, -9.81f, 0));
		Physics->AddSolver(new ImpulseSolver());
		Physics->AddSolver(new SmoothPositionSolver());

		EditorCameraControllerSystem* system = PushSystem<EditorCameraControllerSystem>();

		MainScene->SetMainCamera(system->GetCamera());
		
		PushSystem<PhysicsSystem>();

		PushSystem<iw::MeshShadowRenderSystem>(MainScene);
		PushSystem<iw::MeshRenderSystem>(MainScene);

		PushSystem<iw::SpaceInspectorSystem>();

		Time::SetFixedTime(0.05f);

		SpawnCube();
		SpawnCube(vector3(15, 1, 1));

		// Bush
		iw::Mesh bush = Asset->Load<iw::Model>("models/bush.gltf")->GetMesh(0);
		bush.Material()->SetShader(phong);
		bush.Material()->SetTransparency(iw::Transparency::ADD);

		iw::Entity e = Space->CreateEntity<iw::Transform, iw::Mesh>();

		e.Set<iw::Transform>(iw::vector3(5, 5, 2), 5);
		e.Set<iw::Mesh>(bush);

		return Layer::Initialize();
	}

	void TestLayer::PostUpdate() {

	}

	void TestLayer::FixedUpdate() {
		auto entities = Space->Query<Rigidbody, Mesh, MeshCollider>();

		entities.Each([&](
			auto entity,
			auto body,
			auto mesh,
			auto meshCollider)
		{
			body->Trans().Rotation *= iw::quaternion::from_euler_angles(0, iw::Time::FixedTime(), 0);
			mesh->Material()->Set("albedo", iw::vector4(1, 1, 1, 1));
		});

		if (Keyboard::KeyDown(E)) {
			SpawnCube();
		}

		Physics->Step(Time::FixedTime());


		//auto entities = Space->Query<Transform, Rigidbody>();


		//int i = 0;

		//entities.Each([&](
		//	auto entity, 
		//	auto transform,
		//	auto rigidbody)
		//{
		//	if (i == 0) {
		//		//MainScene->PointLights()[0]->SetPosition(transform->Position);
		//	}

		//	i++;
		//	entities.Each([&](
		//		auto entity2,
		//		auto transform2,
		//		auto rigidbody2)
		//	{
		//			vector3 dir = transform2->Position - transform->Position;
		//			float grav = rigidbody->Mass() * rigidbody2->Mass() / (1 + dir.length_squared());

		//			rigidbody->ApplyForce ( dir.normalized() * grav);
		//			rigidbody2->ApplyForce(-dir.normalized() * grav);
		//	});
		//});

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

