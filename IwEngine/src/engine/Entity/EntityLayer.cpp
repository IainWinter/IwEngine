#include "iw/engine/Entity/EntityLayer.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/log/logger.h"
#include "imgui/imgui.h"
#include "iw/util/io/File.h"
#include "iw/engine/Time.h"

namespace IwEngine {
	EntityLayer::EntityLayer()
		: Layer("Entity")
		, viewTransform(iwm::matrix4::identity)
		, projTransform(iwm::matrix4::create_perspective_field_of_view(
			1.2f, 1.78f, 0.1f, 1000.0f))
		, lightColor(iwm::vector3::one)
		, lightAngle(0.0f)
		, specularScale(0.0f)
	{}

	EntityLayer::~EntityLayer() {}

	int EntityLayer::Initialize() {
		device = new IwRenderer::GLDevice();

		IwGraphics::ModelLoader loader;
		IwGraphics::ModelData* obj = loader.Load("res/bear.obj");

		for (size_t i = 0; i < obj->MeshCount; i++) {
			IwRenderer::IIndexBuffer* ib = device->CreateIndexBuffer(
				obj->Indices[i].FaceCount,
				obj->Indices[i].Faces);

			IwRenderer::VertexBufferLayout* vbl
				= new IwRenderer::VertexBufferLayout();
			vbl->Push<float>(3);
			vbl->Push<float>(3);

			IwRenderer::IVertexBuffer* vb = device->CreateVertexBuffer(
				obj->Meshes[i].VertexCount * sizeof(IwGraphics::Vertex),
				obj->Meshes[i].Vertices);

			IwRenderer::IVertexArray* va
				= device->CreateVertexArray(1, &vb, &vbl);

			modelMeshes.push_back({ va, ib, obj->Indices[i].FaceCount });
		}

		model = space.CreateEntity();
		space.CreateComponent<Transform>(model);
		space.CreateComponent<Velocity>(model);

		std::get<0>(space.GetComponents<Transform>())->Position.z -= 10.0f;

		IwRenderer::IVertexShader* vs = device->CreateVertexShader(
			IwUtil::ReadFile("res/defaultvs.glsl").c_str());

		IwRenderer::IFragmentShader* fs = device->CreateFragmentShader(
			IwUtil::ReadFile("res/defaultfs.glsl").c_str());

		pipeline = device->CreatePipeline(vs, fs);

		return 0;
	}

	void EntityLayer::Destroy() {
		for (auto& mesh : modelMeshes) {
			device->DestroyVertexArray(mesh.Vertices);
			device->DestroyIndexBuffer(mesh.Indices);
		}

		device->DestroyPipeline(pipeline);

		//space.DestroyEntity(model);
	}

	void EntityLayer::Update() {
		lightAngle += Time::DeltaTime();

		float x = cos(lightAngle) * 100;
		float z = sin(lightAngle) * 100;

		auto components = space.GetComponents<Transform, Velocity>();

		Transform& modelTransform = *std::get<0>(components);
		Velocity&  modelVelocity  = *std::get<1>(components);

		device->SetPipeline(pipeline);

		pipeline->GetParam("model")
			->SetAsMat4(modelTransform.GetTransformation());

		pipeline->GetParam("view")
			->SetAsMat4(viewTransform);

		pipeline->GetParam("proj")
			->SetAsMat4(projTransform);

		pipeline->GetParam("lightPos")
			->SetAsVec3(iwm::vector3(x, 0, z));

		pipeline->GetParam("lightColor")
			->SetAsVec3(lightColor);

		pipeline->GetParam("specularScale")
			->SetAsFloat(specularScale);

		for (auto& mesh : modelMeshes) {
			device->SetVertexArray(mesh.Vertices);
			device->SetIndexBuffer(mesh.Indices);
			device->DrawElements(mesh.Count, 0);
		}

		modelTransform.Position += modelVelocity.Velocity;
	}

	void EntityLayer::ImGui() {
		ImGui::Begin("Entity layer");

		auto components = space.GetComponents<Transform>();
		Transform& transform = *std::get<0>(components);

		ImGui::Text("Pos (x, y, z): %f %f %f", transform.Position);
		ImGui::Text("Rot (y): %f", transform.Rotation.euler_angles().y);

		ImGui::SliderFloat3("Light color", &lightColor.x, 0, 1);
		ImGui::SliderFloat("Specular scale", &specularScale, 0, 10);

		ImGui::End();
	}

	bool EntityLayer::On(
		WindowResizedEvent& event)
	{
		return false;
	}

	bool EntityLayer::On(
		MouseMovedEvent& event)
	{
		return false;
	}

	bool EntityLayer::On(
		MouseButtonEvent& event)
	{
		auto components = space.GetComponents<Velocity>();
		Velocity& velocity = *std::get<0>(components);

		float speed = event.State ? Time::DeltaTime() * 15 : 0.0f;

		if (event.Button == IwInput::MOUSE_L_BUTTON) {
			velocity.Velocity.z = -speed;
		}

		else if (event.Button == IwInput::MOUSE_R_BUTTON) {
			velocity.Velocity.z = speed;
		}

		else if (event.Button == IwInput::MOUSE_X1_BUTTON) {
			velocity.Velocity.x = speed;
		}

		else if (event.Button == IwInput::MOUSE_X2_BUTTON) {
			velocity.Velocity.x = -speed;
		}

		return false;
	}

	bool EntityLayer::On(
		MouseWheelEvent& event)
	{
		auto components = space.GetComponents<Transform>();
		Transform& transform = *std::get<0>(components);

		transform.Rotation *= iwm::quaternion::create_from_euler_angles(
			0, event.Delta * .1f, 0);

		return false;
	}
}
