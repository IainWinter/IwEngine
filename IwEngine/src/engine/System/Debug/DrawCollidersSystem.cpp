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

		material = REF<Material>();
		
		material->Set("color", iw::Color(0, 1, 0, 1));
		material->SetWireframe(true);
		material->SetShader(shader);

		MeshData* sphere  = MakeIcosphere(description, 5);
		MeshData* plane   = MakePlane    (description, 5, 5);

		sphere ->Initialize(Renderer->Device); // this should happen automatically when trying to render an uninitialized mesh
		plane  ->Initialize(Renderer->Device);

		sphereInstance = sphere->MakeInstance();
		sphereInstance.SetMaterial(material);

		planeInstance = plane->MakeInstance();
		planeInstance.SetMaterial(material);

		return 0;
	}

	void DrawCollidersSystem::Update() {
		Renderer->BeginScene(m_camera);
		
		for (CollisionObject* object : Physics->CollisionObjects()) {
			if (!object->Col()) continue;
			
			switch (object->Col()->Type()) {
				case ColliderType::SPHERE: {
					Renderer->DrawMesh(object->ColTrans(), sphereInstance);
					break;
				}
				case ColliderType::CAPSULE: {
					CapsuleCollider* col = (CapsuleCollider*)object->Col();

					auto key = std::make_pair(col->Height, col->Radius);
					auto itr = capsules.find(key);
					if (itr == capsules.end()) {
						iw::Mesh mesh = MakeCapsule(description, 5, col->Height, col->Radius)->MakeInstance();

						itr = capsules.emplace(key, ).first;
						itr->second.Data()->Initialize(Renderer->Device);
						itr->second.SetMaterial(material);
					}		

					Renderer->DrawMesh(object->ColTrans(), itr->second);

					break;
				}
				case ColliderType::PLANE: {
					Renderer->DrawMesh(object->ColTrans(), planeInstance);
					break; 
				}
			}
		}

		Renderer->EndScene();	
	}
}
}
