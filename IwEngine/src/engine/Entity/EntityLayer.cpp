#include "iw/engine/Entity/EntityLayer.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/log/logger.h"
#include "imgui/imgui.h"
#include "iw/util/io/File.h"
#include "iw/engine/Time.h"

#include "iw/physics/Collision/BoxCollider.h"
#include "iw/physics/Collision/Algorithm/GJK.h"

namespace IwEngine {
	EntityLayer::EntityLayer()
		: Layer("Entity")
		, lightColor(iwm::vector3::one)
		, lightAngle(0.0f)
		, specularScale(0.0f)
		, view(space.ViewComponents<Transform, Velocity, Model>())
	{}

	EntityLayer::~EntityLayer() {}

	Model LoadModel(
		const char* name,
		IwGraphics::ModelLoader& loader,
		IwRenderer::IDevice* device) 
	{
		IwGraphics::ModelData* obj = loader.Load(name);

		Mesh* meshes = new Mesh[obj->MeshCount];

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

			meshes[i] = Mesh {va, ib, obj->Indices[i].FaceCount };
		}

		return { meshes, obj->MeshCount };
	}

	void EntityLayer::CreateCube(float x, float y, float z, Model& model) {
		IwEntity::Entity e = space.CreateEntity();
		
		Transform& transform = space.CreateComponent<Transform>(e);
		transform.Position.x = x;
		transform.Position.y = y;
		transform.Position.z = z;

		space.CreateComponent<Velocity>(e);
		space.CreateComponent<Model>(e, model);
		space.CreateComponent<IwPhysics::BoxCollider>(
			e, IwPhysics::AABB(iwm::vector3(0, 0, 0), 1));
	}

	int EntityLayer::Initialize() {
		//Create rendering device
		device = new IwRenderer::GLDevice();

		IwEntity::Entity camera = space.CreateEntity();
		space.CreateComponent<Transform>(camera);
		space.CreateComponent<Velocity>(camera);
		space.CreateComponent<Camera>(camera, 
			iwm::matrix4::create_perspective_field_of_view(
				1.2f, 1.78f, 0.1f, 1000.0f));

		space.Sort();

		//Creating shader pipeline
		IwRenderer::IVertexShader* vs = device->CreateVertexShader(
			iwu::ReadFile("res/defaultvs.glsl").c_str());

		IwRenderer::IFragmentShader* fs = device->CreateFragmentShader(
			iwu::ReadFile("res/defaultfs.glsl").c_str());

		pipeline = device->CreatePipeline(vs, fs);

		return 0;
	}

	void EntityLayer::Destroy() {
		auto view = space.ViewComponents<Model>();
		for (auto entity : view) {
			Model& model = entity.GetComponent<Model>();
			for (int i = 0; i < model.MeshCount; i++) {
				device->DestroyVertexArray(model.Meshes[i].Vertices);
				device->DestroyIndexBuffer(model.Meshes[i].Indices);
			}
		}

		device->DestroyPipeline(pipeline);
	}

	void EntityLayer::Update() {
		lightAngle += Time::DeltaTime() * .1f;

		float x = cos(lightAngle) * 100;
		float z = sin(lightAngle) * 100;

		device->SetPipeline(pipeline);
		
		for (auto movement : space.ViewComponents<Transform, Velocity>()) {
			Transform& transform = movement.GetComponent<Transform>();
			Velocity& velocity   = movement.GetComponent<Velocity>();

			transform.Position -= velocity.Velocity;
		}

		for (auto players : space.ViewComponents<Transform, Camera>()) {
			Transform& transform = players.GetComponent<Transform>();
			Camera& camera       = players.GetComponent<Camera>();

			pipeline->GetParam("view")
				->SetAsMat4(transform.GetTransformation());

			pipeline->GetParam("proj")
				->SetAsMat4(camera.Projection);

			pipeline->GetParam("lightPos")
				->SetAsVec3(iwm::vector3(x, 0, z));

			pipeline->GetParam("lightColor")
				->SetAsVec3(lightColor);

			pipeline->GetParam("specularScale")
				->SetAsFloat(specularScale);
		}

		for (auto entity : space.ViewComponents<Transform, Model>()) {
			Transform& t = entity.GetComponent<Transform>();
			Model& model = entity.GetComponent<Model>();

			pipeline->GetParam("model")
				->SetAsMat4(t.GetTransformation());

			for (int i = 0; i < model.MeshCount; i++) {
				device->SetVertexArray(model.Meshes[i].Vertices);
				device->SetIndexBuffer(model.Meshes[i].Indices);
				device->DrawElements(model.Meshes[i].Count, 0);
			}
		}
	}

	void EntityLayer::ImGui() {
		ImGui::Begin("Entity layer");

		for (auto entity : space.ViewComponents<Transform>()) {
			Transform& transform = entity.GetComponent<Transform>();

			ImGui::Text("Pos (x, y, z): %f %f %f",
				transform.Position.x,
				transform.Position.y,
				transform.Position.z);

			iwm::vector3 rot = transform.Rotation.euler_angles();

			ImGui::Text("Rot (x, y, z): %f %f %f",
				rot.x,
				rot.y,
				rot.z);
		}

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
		auto p = *space.ViewComponents<Transform, Camera>().begin();
		Transform& transform = p.GetComponent<Transform>();

		iwm::vector3 euler = transform.Rotation.euler_angles();
		euler.y += event.DeltaX * Time::DeltaTime();
		euler.x += event.DeltaY * Time::DeltaTime();

		transform.Rotation = iwm::quaternion::create_from_euler_angles(euler);

		return false;
	}

	bool EntityLayer::On(
		MouseButtonEvent& event)
	{
		if (event.State && event.Button == IwInput::MOUSE_L_BUTTON) {
			Model m = LoadModel("res/cube.obj", loader, device);

			auto p = *space.ViewComponents<Transform, Camera>().begin();
			Transform& transform = p.GetComponent<Transform>();

			CreateCube(transform.Position.x, transform.Position.y, transform.Position.z - 5, m);

			space.Sort();

			view = space.ViewComponents<Transform, Velocity, Model>();
		}

		return false;
	}

	bool EntityLayer::On(
		MouseWheelEvent& event)
	{
		for (auto entity : view) {
			Transform& transform = entity.GetComponent<Transform>();

			transform.Rotation *= iwm::quaternion::create_from_euler_angles(
				0, event.Delta * .1f, 0);
		}

		return false;
	}

	bool EntityLayer::On(
		KeyEvent& event)
	{
		float delta = event.State ? 15 * Time::DeltaTime() : 0;
		for (auto players : space.ViewComponents<Transform, Velocity, Camera>()) {
			Transform& transform = players.GetComponent<Transform>();
			Velocity& velocity   = players.GetComponent<Velocity>();

			if (event.Button == IwInput::W) {
				velocity.Velocity = iwm::vector3::unit_z
					* transform.Rotation
					* delta;
			}

			if (event.Button == IwInput::S) {
				velocity.Velocity = -iwm::vector3::unit_z
					* transform.Rotation
					* delta;
			}

			if (event.Button == IwInput::D) {
				velocity.Velocity = iwm::vector3::unit_x
					* transform.Rotation
					* delta;
			}

			if (event.Button == IwInput::A) {
				velocity.Velocity = -iwm::vector3::unit_x
					* transform.Rotation
					* delta;
			}

			if (event.Button == IwInput::SPACE) {
				velocity.Velocity = iwm::vector3::unit_y
					* delta;
			}

			if (event.Button == IwInput::SHIFT) {
				velocity.Velocity = -iwm::vector3::unit_y
					* delta;
			}
		}

		return false;
	}
}
