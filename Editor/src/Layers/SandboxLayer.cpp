#include "Layers/SandboxLayer.h"
#include "iw/graphics/Model.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Time.h"

#include "iw/physics/Collision/SphereCollider.h"

#include "Events/ActionEvents.h"

namespace IW {
	struct ModelComponents {
		Transform* Transform;
		Model* Model;
	};

	struct CameraComponents {
		Transform* Transform;
		CameraController* Controller;
	};

	SandboxLayer::SandboxLayer()
		: Layer("Sandbox")
	{}

	int SandboxLayer::Initialize() {
		space.SetGravity(iw::vector3(0, -1.8f, 0));

		return 0;
	}

	void SandboxLayer::PostUpdate() {
		Renderer->BeginScene();

		for (auto c_e : Space->Query<Transform, CameraController>()) {
			auto [c_t, c_c] = c_e.Components.Tie<CameraComponents>();

			for (auto m_e : Space->Query<Transform, Model>()) {
				auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();

				for (size_t i = 0; i < m_m->MeshCount; i++) {
					Mesh& mesh = m_m->Meshes[i];

					mesh.Material->Shader->Program->GetParam("view")
						->SetAsMat4(c_c->Camera->GetView());

					mesh.Material->Shader->Program->GetParam("proj")
						->SetAsMat4(c_c->Camera->GetProjection());

					Renderer->DrawMesh(m_t, &mesh);
				}
			}
		}

		Renderer->EndScene();
	}

	void SandboxLayer::FixedUpdate() {
		space.Step(Time::FixedTime());
	}

	float z = 0;

	bool SandboxLayer::On(
		ActionEvent& e)
	{
		if (e.Action != iw::val(Actions::SPAWN_CIRCLE_TEMP))
			return false;

		iw::ref<Model> sphere = Asset->Load<Model>("Sphere");

		Entity ent = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody>();

		Space->SetComponentData<Model>(ent, *sphere);

		Transform*      t = Space->SetComponentData<Transform>     (ent, iw::vector3(0, 0, z+= 0.1f));
		SphereCollider* s = Space->SetComponentData<SphereCollider>(ent, iw::vector3::zero, 1.0f);
		Rigidbody*      r = Space->SetComponentData<Rigidbody>     (ent);

		r->SetTakesGravity(true);
		r->SetSimGravity(true);
		r->SetMass(1);
		r->ApplyForce(iw::vector3(0, 30, 0));
		r->SetCol(s);
		r->SetTrans(t);

		space.AddRigidbody(r);

		return true;
	}
}

