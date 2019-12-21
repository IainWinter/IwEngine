#include "Layers/SandboxLayer.h"
#include "iw/graphics/Model.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Time.h"

#include "iw/physics/Collision/SphereCollider.h"

#include "Events/ActionEvents.h"

#include "imgui/imgui.h"

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
		space.SetGravity(iw::vector3(0, -9.8f, 0));

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

	float ts = 0.1f;
	
	void SandboxLayer::FixedUpdate() {
		space.Step(Time::FixedTime() * ts);
	}

	void SandboxLayer::ImGui() {
		ImGui::Begin("Sandbox");

		ImGui::SliderFloat("Time scale", &ts, 0.001f, 1);

		ImGui::End();
	}

	float x = 0;

	bool SandboxLayer::On(
		ActionEvent& e)
	{
		if (e.Action != iw::val(Actions::SPAWN_CIRCLE_TEMP))
			return false;

		iw::ref<Model> sphere = Asset->Load<Model>("Sphere");

		for (size_t i = 0; i < 5; i++) {
			Entity ent = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody>();

			Space->SetComponentData<Model>(ent, *sphere);

			Transform*      t = Space->SetComponentData<Transform>     (ent, iw::vector3(0, 0, 0));
			SphereCollider* s = Space->SetComponentData<SphereCollider>(ent, iw::vector3::zero, 1.0f);
			Rigidbody*      r = Space->SetComponentData<Rigidbody>     (ent);

			r->SetTakesGravity(true);
			r->SetSimGravity(true);
			r->SetMass(1);
			r->ApplyForce(iw::vector3(cos(x += .1f) * 50, 500, sin(x / .1f) * 50));
			//r->ApplyForce(iw::vector3(cos(z) * 30, 100, sin(z += iw::PI * 0.01f) * 30));
			r->SetCol(s);
			r->SetTrans(t);

			space.AddRigidbody(r);
		}
		return true;
	}
}

