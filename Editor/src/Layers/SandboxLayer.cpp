#include "Layers/SandboxLayer.h"
#include "iw/graphics/Model.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Time.h"

#include "iw/physics/Collision/SphereCollider.h"

#include "Events/ActionEvents.h"

#include "imgui/imgui.h"

#include "iw/physics/Dynamics/ManifoldSolver.h"

#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Dynamics/Rigidbody.h"

#include "iw/graphics/MeshFactory.h"

#include "iw/input/Devices/Mouse.h"

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
		space.AddSolver(new ManifoldSolver());

		Entity ent = Space->CreateEntity<Transform, Model, PlaneCollider, Rigidbody>();

		iw::ref<Model> plane = Asset->Load<Model>("Plane");

		Space->SetComponentData<Model>(ent, *plane);

		Transform*     t = Space->SetComponentData<Transform>(ent, iw::vector3(0, 0, 0), iw::vector3(15, 1, 15), iw::quaternion::identity);
		PlaneCollider* s = Space->SetComponentData<PlaneCollider>(ent, iw::vector3::unit_y, 0.0f);
		Rigidbody*     r = Space->SetComponentData<Rigidbody>(ent);

		r->SetTakesGravity(false);
		r->SetSimGravity(false);
		r->SetMass(1);
		r->SetCol(s);
		r->SetTrans(t);

		space.AddRigidbody(r);

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

		for (auto p_e : Space->Query<Transform, PlaneCollider>()) {
			auto [p_t, p_p] = p_e.Components.TieTo<Transform, PlaneCollider>();

			if (IW::Mouse::ButtonDown(IW::XMOUSE1)) {
				p_t->Rotation *= iw::quaternion::from_euler_angles(iw::PI, 0, 0);
			}

			if (IW::Mouse::ButtonDown(IW::XMOUSE2)) {
				p_t->Rotation *= iw::quaternion::from_euler_angles(iw::PI, 0, 0);
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
	int sc = 1;

	bool SandboxLayer::On(
		ActionEvent& e)
	{
		if (e.Action != iw::val(Actions::SPAWN_CIRCLE_TEMP))
			return false;

		iw::ref<Model> sphere = Asset->Load<Model>("Sphere");

		for (size_t i = 0; i < sc; i++) {
			Entity ent = Space->CreateEntity<Transform, Model, SphereCollider, Rigidbody>();

			Space->SetComponentData<Model>(ent, *sphere);

			Transform*      t = Space->SetComponentData<Transform>     (ent, iw::vector3(0, 5, 0));
			SphereCollider* s = Space->SetComponentData<SphereCollider>(ent, iw::vector3::zero, 1.0f);
			Rigidbody*      r = Space->SetComponentData<Rigidbody>     (ent);

			r->SetTakesGravity(true);
			r->SetSimGravity(true);
			r->SetMass(1);
			//r->ApplyForce(iw::vector3(cos(x += .1f) * 50, 500, sin(x / .1f) * 50));
			r->ApplyForce(iw::vector3(cos(x) * 30, 100, sin(x += 2 * iw::PI / sc) * 30));
			r->SetCol(s);
			r->SetTrans(t);

			space.AddRigidbody(r);
		}
		return true;
	}
}

