#include "App.h"

#include "Layers/ToolLayer.h"
#include "Layers/SandboxLayer.h"
#include "Events/ActionEvents.h"
#include "iw/physics/Plane.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"

namespace IW {
	App::App() {
		iw::ref<Context> context = Input->CreateContext("Sandbox");
		
		context->MapButton(IW::SPACE, "+jump");
		context->MapButton(IW::SHIFT, "-jump");
		context->MapButton(IW::D    , "+right");
		context->MapButton(IW::A    , "-right");
		context->MapButton(IW::W    , "+forward");
		context->MapButton(IW::S    , "-forward");
		context->MapButton(IW::E    , "use");

		iw::ref<Device> m  = Input->CreateDevice<Mouse>();
		iw::ref<Device> rm = Input->CreateDevice<RawMouse>();
		iw::ref<Device> k  = Input->CreateDevice<RawKeyboard>();
		
		context->AddDevice(m);
		context->AddDevice(rm);
		context->AddDevice(k);

		active = false;
		toolbox = PushLayer<ToolLayer>();
		PushLayer<SandboxLayer>();
	}

	int App::Initialize(
		IW::InitOptions& options)
	{
		int err = Application::Initialize(options);

		if (!err) {
			GetLayer<ImGuiLayer>("ImGui")->BindContext();
		}

		PopLayer(toolbox);

		return err;
	}

	bool App::HandleCommand(
		const Command& command)
	{
		if (command.Verb == "jump") {
			Bus->push<JumpEvent>(command.Active);
		}

		else if (command.Verb == "right") {
			Bus->push<RightEvent>(command.Active);
		}

		else if (command.Verb == "forward") {
			Bus->push<ForwardEvent>(command.Active);
		}

		else if (command.Verb == "/") {
			if (active) {
				PopLayer(toolbox);
			}

			else {
				PushLayer(toolbox);
			}

			active = !active;
		}

		return Application::HandleCommand(command);
	}

	void App::HandleCollision(
		Manifold& manifold,
		scalar dt)
	{
		Entity a = Space->FindEntity(manifold.BodyA);
		Entity b = Space->FindEntity(manifold.BodyB);

		if (a.Handle.Index != EntityHandle::Empty.Index) {
			Rigidbody* r = a.FindComponent<Rigidbody>();

			Physics->RemoveRigidbody(r);

			a.AddComponent<CollisionEvent>(manifold.BodyA, manifold.BodyB, manifold.PenetrationDepth, dt);

			r = a.FindComponent<Rigidbody>();
			r->SetTrans(a.FindComponent<Transform>());
			r->SetCol(a.FindComponent<SphereCollider>());

			Physics->AddRigidbody(r);
		}

		if (b.Handle.Index != EntityHandle::Empty.Index) {
			//Rigidbody* r = b.FindComponent<Rigidbody>();

			//Physics->RemoveRigidbody(r);

			//b.AddComponent<CollisionEvent>(manifold.BodyB, manifold.BodyA, -manifold.PenetrationDepth, dt);

			//r = b.FindComponent<Rigidbody>();
			//r->SetTrans(b.FindComponent<Transform>());

			//Collider* c = b.FindComponent<SphereCollider>();
			//if (!c) {
			//	c = b.FindComponent<PlaneCollider>();
			//}

			//r->SetCol(c);

			//Physics->AddRigidbody(r);
		}

		Application::HandleCollision(manifold, dt);
	}
}

IW::Application* CreateApplication(
	IW::InitOptions& options)
{
	options.WindowOptions = IW::WindowOptions {
		1280,
		720,
		true,
		IW::DisplayState::NORMAL
	};

	return new IW::App();
}
