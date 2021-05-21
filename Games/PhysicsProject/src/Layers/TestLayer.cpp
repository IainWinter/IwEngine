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

#include "iw/physics/Dynamics/Mechanism.h"

#include <iw\engine\Systems\PhysicsSystem.h>
#include "iw/input/Devices/Keyboard.h"
#include "iw/physics/Collision/PositionSolver.h"
#include "iw/graphics/MeshFactory.h"

#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include "iw/engine/Systems/Render/RenderSystem.h"
#include "iw/engine/Systems/Render/ShadowRenderSystem.h"
//#include "Systems/SpaceInspectorSystem.h"

#include "glm/gtc/random.hpp"
#include "glm/gtx/matrix_cross_product.hpp"

//#include "iw/reflection/serialization/JsonSerializer.h"

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

		//if (x > 0) {
			col = entity.Add<MeshCollider>(MeshCollider::MakeCube());
			mesh = entity.Set<Mesh>(cube->MakeInstance());
		//}

		//else if (randf() > 0.0f) {
		//	col  = entity.Add<MeshCollider>(MeshCollider::MakeTetrahedron());
		//	mesh = entity.Set<Mesh>(tetrahedron->MakeInstance());
		//}

		//else {
		//	col = entity.Add<SphereCollider>(glm::vec3(0), 1);
		//	mesh = entity.Set<Mesh>(sphere->MakeInstance());

		//	s.x = s.z;
		//	s.y = s.z; // make uniform scale if where because thats how their collider works
		//}

		x += 5;

		Transform*      trans = entity.Set<Transform>(glm::vec3(0, 15, 0), s/*, iw::quaternion::from_euler_angles(randf() + 1, randf() + 1, randf() + 1)*/);
		Rigidbody*      body  = entity.Set<Rigidbody>();

		//trans->Rotation = quaternion::from_euler_angles(iw::randf() * iw::Pi2, iw::randf() * iw::Pi2, iw::randf() * iw::Pi2);
		trans->Position = glm::ballRand(10.f);

		mesh->SetMaterial(REF<Material>(shader));

		mesh->Material()->Set("albedo", glm::vec4(
			glm::ballRand(.5f) + glm::vec3(.5f),
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
			body->IsAxisLocked = glm::vec3(1);
			body->AxisLock = glm::vec3(0, 15, 0);
			body->SimGravity = false;

			body->Restitution = 0;
			body->DynamicFriction = 1;
			body->StaticFriction  =1;
		}
		else {
			//body->Velocity = glm::ballRand(5.f);
			//body->AngularVelocity = glm::ballRand(5.f);
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

			Transform*       trans = Ground.Set<Transform>(glm::vec3(0, -2, 0), glm::vec3(22)); // for project idk why if this isnt here the lights break
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

		DirectionalLight* dirLight = new DirectionalLight(10, OrthographicCamera(64, 64, -100, 100));
		dirLight->SetRotation(glm::lookAt(glm::vec3(), glm::vec3(-1, -5, -1), glm::vec3(0, 1, 0)));
		dirLight->SetShadowShader(dirShadowShader);
		dirLight->SetShadowTarget(dirShadowTarget);

		// scene

		MainScene->AddLight(dirLight);

		//delete sphere;

		//Physics->SetGravity(glm::vec3(0, -9.81f, 0));
		//Physics->AddSolver(new ImpulseSolver());
		//Physics->AddSolver(new SmoothPositionSolver());

		//Time::SetFixedTime(0.02f);
		//Time::SetTimeScale(0.01f);

		EditorCameraControllerSystem* system = PushSystem<EditorCameraControllerSystem>();

		MainScene->SetMainCamera(system->GetCamera());
		
		PushSystem<PhysicsSystem>();
		PushSystem<iw::ShadowRenderSystem>(MainScene);
		PushSystem<iw::RenderSystem>(MainScene);

		srand(19);

		/*Entity last = SpawnCube(glm::vec3(.1));
		Entity current;*/

		//iw::Mechanism* mechanism = new iw::Mechanism();

		int demo = 1;
		if (demo == 0)
		{
			Entity a = SpawnCube(glm::vec3(.2));
			Entity b = SpawnCube(glm::vec3(2));
			Entity c = SpawnCube(glm::vec3(2));
		
			Entity d = SpawnCube(glm::vec3(2));
			Entity e = SpawnCube(glm::vec3(2));
			Entity f = SpawnCube(glm::vec3(2));
			Entity g = SpawnCube(glm::vec3(2));

			VelocityConstraint* ab = new BallInSocketConstraint(
				a.Find<iw::Rigidbody>(),
				b.Find<iw::Rigidbody>(),
				from_glm(glm::vec3( 0)), 
				from_glm(glm::vec3(-1))
			);

			a.Find<iw::Rigidbody>()->IsAxisLocked = glm::vec3(1);
			a.Find<iw::Rigidbody>()->AxisLock     = glm::vec3(0, 10, 0);
		
			VelocityConstraint* bc = new BallInSocketConstraint(
				b.Find<iw::Rigidbody>(),
				c.Find<iw::Rigidbody>(),
				from_glm(glm::vec3( 1)), 
				from_glm(glm::vec3(-1))
			);

			VelocityConstraint* cd = new BallInSocketConstraint(
				c.Find<iw::Rigidbody>(),
				d.Find<iw::Rigidbody>(),
				from_glm(glm::vec3(1)),
				from_glm(glm::vec3(-1))
			);

			VelocityConstraint* de = new BallInSocketConstraint(
				d.Find<iw::Rigidbody>(),
				e.Find<iw::Rigidbody>(),
				from_glm(glm::vec3(1)),
				from_glm(glm::vec3(-1))
			);

			VelocityConstraint* ef = new BallInSocketConstraint(
				e.Find<iw::Rigidbody>(),
				f.Find<iw::Rigidbody>(),
				from_glm(glm::vec3(1)),
				from_glm(glm::vec3(-1))
			);

			VelocityConstraint* fg = new BallInSocketConstraint(
				f.Find<iw::Rigidbody>(),
				g.Find<iw::Rigidbody>(),
				from_glm(glm::vec3(1)),
				from_glm(glm::vec3(-1))
			);

			Physics->AddConstraint(ab);
			Physics->AddConstraint(bc);
			Physics->AddConstraint(cd);
			Physics->AddConstraint(de);
			Physics->AddConstraint(ef);
			Physics->AddConstraint(fg);

			Ground.Find<iw::CollisionObject>()->Trans().Position.y = -200;

			Physics->SetGravity(glm::vec3(0, -9.81f, 0));
		}
		
		else
		if (demo == 1)
		{
			Entity a = SpawnCube(glm::vec3(.5, .5, 5));
			Entity b = SpawnCube(glm::vec3(2));
			Entity c = SpawnCube(glm::vec3(.5, .5, 5));
			Entity d = SpawnCube(glm::vec3(2));

			VelocityConstraint* ab = new BallInSocketConstraint(
				a.Find<iw::Rigidbody>(),
				b.Find<iw::Rigidbody>(),
				from_glm(glm::vec3( 1, 1, -1)), 
				from_glm(glm::vec3(-1, 1, 1))
			);

			VelocityConstraint* bc = new BallInSocketConstraint(
				b.Find<iw::Rigidbody>(),
				c.Find<iw::Rigidbody>(),
				from_glm(glm::vec3( 1)), 
				from_glm(glm::vec3(-1, 1, -1))
			);

			VelocityConstraint* cd = new BallInSocketConstraint(
				c.Find<iw::Rigidbody>(),
				d.Find<iw::Rigidbody>(),
				from_glm(glm::vec3(-1, 1, 1)),
				from_glm(glm::vec3(1, 1, -1))
			);

			VelocityConstraint* da = new BallInSocketConstraint(
				d.Find<iw::Rigidbody>(),
				a.Find<iw::Rigidbody>(),
				from_glm(glm::vec3(-1, 1, -1)),
				from_glm(glm::vec3(1))
			);

			Physics->AddConstraint(ab);
			Physics->AddConstraint(bc);
			Physics->AddConstraint(cd);
			Physics->AddConstraint(da);

			b.Find<iw::Rigidbody>()->Gravity.y = -10;
			d.Find<iw::Rigidbody>()->Gravity.y = 5;

			Physics->AddSolver(new ImpulseSolver());
			Physics->AddSolver(new SmoothPositionSolver());
		}

		//Constraint* bc = new VelocityConstraint(glm::vec3(-1), glm::vec3(-1));
		//Constraint* cd = new VelocityConstraint(glm::vec3(-1), glm::vec3(-1));
		//Constraint* da = new VelocityConstraint(glm::vec3(-1), glm::vec3(-1));

		//ab->A = a.Find<Rigidbody>();
		//ab->B = b.Find<Rigidbody>();
		//
		//bc->A = b.Find<Rigidbody>();
		//bc->B = c.Find<Rigidbody>();
		//
		//cd->A = c.Find<Rigidbody>();
		//cd->B = d.Find<Rigidbody>();
		//
		//da->A = d.Find<Rigidbody>();
		//da->B = a.Find<Rigidbody>();

		//mechanism->Constraints.push_back(ab);
		//mechanism->Constraints.push_back(bc);
		//mechanism->Constraints.push_back(cd);
		//mechanism->Constraints.push_back(da);

		//for (int i = 0; i < 5; i++) {

		//	current = SpawnCube();

		//	Constraint* c = new VelocityConstraint(glm::vec3(i == 0 ? 0 : -1), glm::vec3(1));
		//	c->A = last   .Find<iw::Rigidbody>();
		//	c->B = current.Find<iw::Rigidbody>();

		//	mechanism->Constraints.push_back(c);

		//	last = current;
		//}

		//Physics->AddMechanism(mechanism);

		//TestDebug = SpawnCube(glm::vec3(.1));
		//TestDebug.Find<iw::Rigidbody>()->IsKinematic = false;

		return Layer::Initialize();
	}

	void TestLayer::CorrectVelocity(Entity entity, Entity target, glm::vec3 offset) {
		//using namespace glm;

		//Transform targetTrans = target.Find<Rigidbody>()->Trans();

		//vec3& targetPos = targetTrans.Position + targetTrans.Rotation * (targetTrans.Scale * offset);

		//float beta = .1f;
		//float dt = iw::FixedTime();
		//
		//Transform* transform = &entity.Find<iw::Rigidbody>()->Trans();
		//Rigidbody* rigidbody =  entity.Find<iw::Rigidbody>();

		//vec3 r = transform->Rotation * (transform->Scale * vec3(1));

		//vec3 cornerPos = transform->Position + r;
		////TestDebug.Find<iw::Rigidbody>()->Trans().Position = cornerPos;

		//mat3 invMass      = mat3(rigidbody->InvMass);
		//mat3 invInrtLocal = mat3(rigidbody->InvMass);

		//vec4 x = vec4(1.0f, 0.0f, 0.0f, 1.0f) * transform->WorldTransformation();
		//vec4 y = vec4(0.0f, 1.0f, 0.0f, 1.0f) * transform->WorldTransformation();
		//vec4 z = vec4(0.0f, 0.0f, 1.0f, 1.0f) * transform->WorldTransformation();

		//mat3 world2Local = transpose(mat3(
		//	vec3(x.x, x.y, x.z),
		//	vec3(y.x, y.y, y.z),
		//	vec3(z.x, z.y, z.z)
		//));

		//mat3 invInrtWorld = transpose(world2Local) * invInrtLocal * world2Local;

		//vec3 c = cornerPos - targetPos;
		//vec3 v = rigidbody->Velocity + cross(rigidbody->AngularVelocity, r);

		//mat3 s = matrixCross3(-r);
		//mat3 k = invMass + s * invInrtWorld * transpose(s);
		//vec3 lambda = inverse(k) * -(v + c * beta / dt);

		//rigidbody->Velocity += invMass * lambda;
		//rigidbody->Velocity *= 0.98f;

		//rigidbody->AngularVelocity += (invInrtWorld * transpose(s)) * lambda;
		//rigidbody->AngularVelocity *= .98f;
	}


	std::vector<float> angularEnergy;
	std::vector<float> linearEnergy;
	std::vector<float> totalEnergy;
	std::vector<float> totalError;

	void TestLayer::FixedUpdate() {
		//Transform* transform = &A.Find<iw::Rigidbody>()->Trans();

		//glm::vec3 r = transform->Rotation * (transform->Scale * glm::vec3(1));
		//glm::vec3 cornerPos = transform->Position + r;

		//TestDebug.Find<iw::Rigidbody>()->Trans().Position = cornerPos;

		//Z.Find<iw::Rigidbody>()->Velocity.x = cos(iw::TotalTime() / 5);
		//Z.Find<iw::Rigidbody>()->Velocity.z = sin(iw::TotalTime() / 5);

		//Z.Find<iw::Rigidbody>()->Velocity.y = 0;

		//mechanism.init();
		//mechanism.solve();

		//glm::vec3& ball = Ball.Find<iw::Rigidbody>()->Trans().Position;

		//ball.y = 10;
		//ball.x = 10 * sin(iw::TotalTime());
		//ball.z = 10 * cos(iw::TotalTime());

		//for (size_t i = 0; i < 10; i++)
		//{
		//	CorrectVelocity(Box, Ball, glm::vec3(0));
		//	CorrectVelocity(Box2, Box, glm::vec3(-1, -1, -1));
		//	CorrectVelocity(Box3, Box2, glm::vec3(-1, -1, -1));
		//	CorrectVelocity(Box4, Box3, glm::vec3(-1, -1, -1));
		//}

		float linear  = 0;
		float angular = 0;

		float error = 0;

		Space->Query<iw::Rigidbody>().Each([&](
			EntityHandle e,
			iw::Rigidbody* r)
		{
			linear  += 1 / 2.0f * r->Mass() * glm::dot(r->Velocity, r->Velocity);
			angular += 1 / 2.0f * glm::dot(r->Inertia * r->AngularVelocity, r->Inertia * r->AngularVelocity);
		});

		for (iw::VelocityConstraint* constraint : Physics->VelocityConstraints()) {
			error += iw::length((constraint->E * (iw::FixedTime() / constraint->biasStrength)).col_begin(0));
		}

		linearEnergy.push_back(linear);
		angularEnergy.push_back(angular);
		totalEnergy.push_back(linear + angular);
		totalError.push_back(error);


		//if (totalError.size() > 333) {
		//	linearEnergy.erase(linearEnergy.begin());
		//	angularEnergy.erase(angularEnergy.begin());
		//	totalEnergy.erase(totalEnergy.begin());
		//	totalError.erase(totalError.begin());
		//}
	}

	float thresh = .5f;



	void TestLayer::ImGui() {

		ImGui::Begin("Results");

		ImGui::PlotLines("Total error", totalError.data(), totalError.size(), 0, 0, FLT_MAX, FLT_MAX, ImVec2(0, 100));
		ImGui::PlotLines("Linear  kinetic energy", linearEnergy.data(), linearEnergy.size(), 0, 0, FLT_MAX, FLT_MAX, ImVec2(0, 100));
		ImGui::PlotLines("Angular kinetic energy", angularEnergy.data(), angularEnergy.size(), 0, 0, FLT_MAX, FLT_MAX, ImVec2(0, 100));
		ImGui::PlotLines("Total   kinetic energy", totalEnergy.data(), totalEnergy.size(), 0, 0, FLT_MAX, FLT_MAX, ImVec2(0, 100));

		if (ImGui::Button("Save data")) {
			for (int i = 0; i < totalError.size(); i++) {
				LOG_INFO << totalError.at(i) << "," << totalEnergy.at(i);
			}
		}

		ImGui::End();

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
			ImGui::SliderFloat4("Light rot", (float*)&rot, -1, 1);
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
					glm::ballRand(.5f) + glm::vec3(.5f),
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

