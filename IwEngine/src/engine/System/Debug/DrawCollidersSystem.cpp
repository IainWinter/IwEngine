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

		material = REF<Material>();
		
		material->Set("color", iw::Color(0, 1, 0, 1));
		material->SetWireframe(true);
		material->SetShader(shader);

		sphere = MakeIcosphere(2);

		sphere->SetNormals(0, nullptr);
		sphere->SetMaterial(material);
		sphere->Initialize(Renderer->Device);

		plane = MakePlane(5, 5);

		plane->SetNormals(0, nullptr);
		plane->SetUVs    (0, nullptr);
		plane->SetMaterial(material);
		plane->Initialize(Renderer->Device);

		//capsule = ref<Mesh>(MakeCapsule(5));

		//capsule->SetNormals(0, nullptr);
		//capsule->SetUVs    (0, nullptr);
		//capsule->SetMaterial(material);
		//capsule->Initialize(Renderer->Device);

		return 0;
	}

	void DrawCollidersSystem::Update() {
		Renderer->BeginScene(m_camera);
		
		for (CollisionObject* object : Physics->CollisionObjects()) {
			if (!object->Col()) continue;
			
			switch (object->Col()->Type()) {
				case ColliderType::SPHERE:  Renderer->DrawMesh(object->ColTrans(), sphere);  break;
				case ColliderType::CAPSULE: {
					CapsuleCollider* col = (CapsuleCollider*)object->Col();

					capsule = ref<Mesh>(
						MakeCapsule(5, col->Height, col->Radius),
						std::bind(&Mesh::Destroy, capsule, Renderer->Device)
					);

					capsule->SetNormals(0, nullptr);
					capsule->SetUVs(0, nullptr);
					capsule->SetMaterial(material);
					capsule->Initialize(Renderer->Device);

					Renderer->DrawMesh(object->ColTrans(), capsule);

					break;
				}
				case ColliderType::PLANE:   Renderer->DrawMesh(object->ColTrans(), plane);   break;
			}
		}

		Renderer->EndScene();	
	}
}
}
