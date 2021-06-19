#include "iw/engine/Systems/Debug/DrawCollidersSystem.h"
#include "iw/graphics/MeshFactory.h"

#include "iw/physics/Collision/CapsuleCollider.h"

namespace iw {
namespace Engine {
	DrawCollidersSystem::DrawCollidersSystem(
		Camera* camera)
		: System<>("Draw Colliders")
		, m_camera(camera)
	{}

	int DrawCollidersSystem::Initialize() {
		ref<Shader> shader = Asset->Load<Shader>("shaders/debug/wireframe.shader");

		description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));

		material = REF<Material>(shader);
		
		material->Set("color", iw::Color(0, 1, 0, 1));
		material->SetWireframe(true);
		material->SetCastShadows(false);

		material->Initialize(Renderer->Device);

		MeshData* sphere  = MakeIcosphere(description, 2);
		MeshData* plane   = MakePlane    (description, 5, 5);
		MeshData* cube    = MakeCube     (description, 2);

		sphere ->Initialize(Renderer->Device); // this should happen automatically when trying to render an uninitialized mesh
		plane  ->Initialize(Renderer->Device);

		sphereInstance = sphere->MakeInstance();
		sphereInstance.Material = material;

		planeInstance = plane->MakeInstance();
		planeInstance.Material = material;

		cubeInstance = cube->MakeInstance();
		cubeInstance.Material = material;

		return 0;
	}

	void DrawCollidersSystem::Update() {
		Renderer->BeginScene(m_camera);
		
		for (CollisionObject* object : Physics->CollisionObjects()) {
			if (!object->Collider) continue;

			Renderer->DrawMesh(object->Transform, cubeInstance);

			//switch (object->Collider->Type) {
			//	case ColliderType::SPHERE: {
			//		Renderer->DrawMesh(object->ColTrans(), sphereInstance);
			//		break;
			//	}
			//	case ColliderType::CAPSULE: {
			//		Capsule* col = (Capsule*)object->Collider;

			//		auto key = std::make_pair(col->Height, col->Radius);
			//		auto itr = capsules.find(key);
			//		if (itr == capsules.end()) {
			//			iw::Mesh mesh = MakeCapsule(description, 5, col->Height, col->Radius)->MakeInstance();
			//			mesh.Data->Initialize(Renderer->Device);
			//			meshMaterial = (material);

			//			itr = capsules.emplace(key, mesh).first;
			//		}

			//		Renderer->DrawMesh(object->ColTrans(), itr->second);

			//		break;
			//	}
			//	case ColliderType::PLANE: {
			//		Renderer->DrawMesh(object->ColTrans(), planeInstance);
			//		break; 
			//	}
			//	case ColliderType::HULL: {
			//		Renderer->DrawMesh(object->Transform, cubeInstance);
			//	}
			//	case ColliderType::MESH: {
			//		Renderer->DrawMesh(object->Transform, cubeInstance);
			//	}
			//}
		}

		Renderer->EndScene();	
	}
}
}
