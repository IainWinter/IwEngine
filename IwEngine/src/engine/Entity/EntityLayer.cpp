#include "iw/engine/Entity/EntityLayer.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/log/logger.h"
#include "imgui/imgui.h"
#include "iw/util/io/File.h"
#include "iw/engine/Time.h"
#include <iw\input\Devices\Keyboard.h>

//#include "iw/physics/Collision/BoxCollider.h"
//#include "iw/physics/Collision/Algorithm/GJK.h"

namespace IwEngine {
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

			meshes[i] = Mesh{ va, ib, obj->Indices[i].FaceCount };
		}

		return { meshes, obj->MeshCount, iwm::vector3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX) };
	}

	void EntityLayer::CreateCube(
		float x,
		float y,
		float z,
		Model& model)
	{
		IwEntity::Entity e = space.CreateEntity();

		Transform& transform = space.CreateComponent<Transform>(e);
		transform.Position = iwm::vector3(x, y, z);

		space.CreateComponent<Velocity>(e);
		space.CreateComponent<Model>(e, model);
		//space.CreateComponent<IwPhysics::BoxCollider>(
		//	e, IwPhysics::AABB(iwm::vector3(0, 0, 0), 1));
	}

	EntityLayer::EntityLayer()
		: Layer("Entity")
		, lightColor(iwm::vector3::one)
		, specularScale(0.0f)
	{}

	int EntityLayer::Initialize() {
		device = new IwRenderer::GLDevice();

		IwRenderer::IVertexShader* vs = device
			->CreateVertexShader(iwu::ReadFile("res/defaultvs.glsl").c_str());

		IwRenderer::IFragmentShader* fs = device
			->CreateFragmentShader(iwu::ReadFile("res/defaultfs.glsl").c_str());

		pipeline = device->CreatePipeline(vs, fs);

		device->SetPipeline(pipeline);

		CreateCube(0, 0, -5, LoadModel("res/cube.obj", loader, device));
			
		IwEntity::Entity camera = space.CreateEntity();
		space.CreateComponent<Transform>(camera);
		space.CreateComponent<Velocity>(camera);
		space.CreateComponent<Camera>(camera, 
			iwm::matrix4::create_perspective_field_of_view(
				1.19f, 1.78f, .01f, 1000.0f));

		space.Sort();

		return 0;
	}

	void EntityLayer::Destroy() {
	
	}

	void EntityLayer::Update() {
		LOG_INFO << IwInput::Keyboard::KeyDown(IwInput::SPACE);

		for (auto entity : space.ViewComponents<Transform>()) {
			Transform& transform = entity.GetComponent<Transform>();

			transform.Transformation
				= iwm::matrix4::create_from_quaternion(transform.Rotation)
				* iwm::matrix4::create_translation(transform.Position);
		}

		auto player = *space.ViewComponents<Transform, Camera>().begin();
		Transform& playerTransform = player.GetComponent<Transform>();
		Camera& playerCamera       = player.GetComponent<Camera>();

		iwm::vector3 input;
		float delta = Time::DeltaTime() * 5;
		LOG_INFO << IwInput::Keyboard::KeyDown(IwInput::W);

		if (IwInput::Keyboard::KeyDown(IwInput::W)) {
			input.z += delta;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::S)) {
			input.z -= delta;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::D)) {
			input.x += delta;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::A)) {
			input.x -= delta;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::SPACE)) {
			input.y += delta;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::SHIFT)) {
			input.y -= delta;
		}

		playerTransform.Position += playerTransform.Forward() * input.z;
		playerTransform.Position += playerTransform.Right()   * input.x;
		playerTransform.Position += iwm::vector3::unit_y      * input.y;
		
		for (auto entity : space.ViewComponents<Transform, Model>()) {
			Transform& entityTransform = entity.GetComponent<Transform>();
			Model& entityModel = entity.GetComponent<Model>();

			pipeline->GetParam("lightPos")
				->SetAsVec3(playerTransform.Position);

			pipeline->GetParam("lightColor")
				->SetAsVec3(lightColor);

			pipeline->GetParam("objectColor")
				->SetAsVec3(entityModel.Color);

			pipeline->GetParam("specularScale")
				->SetAsFloat(specularScale);

			pipeline->GetParam("proj")
				->SetAsMat4(playerCamera.Projection);

			pipeline->GetParam("view")
				->SetAsMat4(iwm::matrix4::create_look_at(
					playerTransform.Position,
					playerTransform.Position + playerTransform.Forward(),
					playerTransform.Up()));

			pipeline->GetParam("model")
				->SetAsMat4(entityTransform.Transformation);

			for (int i = 0; i < entityModel.MeshCount; i++) {
				device->SetVertexArray(entityModel.Meshes[i].Vertices);
				device->SetIndexBuffer(entityModel.Meshes[i].Indices);
				device->DrawElements(entityModel.Meshes[i].Count, 0);
			}
		}

		/*auto v = space.ViewComponents<Transform, Model, IwPhysics::BoxCollider>();

		for (auto entity1 : v) {
			Transform& transform1 = entity1.GetComponent<Transform>();
			Model& model1 = entity1.GetComponent<Model>();
			IwPhysics::BoxCollider& collider1 = entity1.GetComponent<IwPhysics::BoxCollider>();

			for (auto entity2 : v) {
				Transform& transform2 = entity2.GetComponent<Transform>();
				Model& model2 = entity2.GetComponent<Model>();
				IwPhysics::BoxCollider& collider2 = entity2.GetComponent<IwPhysics::BoxCollider>();

				if (entity1 != entity2) {
					if (collider1.Translated(transform1.Transformation).Intersects(
						collider2.Translated(transform2.Transformation)))
					{
						bool colliding = IwPhysics::GJK(
							collider1, collider2,
							transform1.Transformation,
							transform2.Transformation);

						if (colliding) {
							model1.Color = iwm::vector3(.3f, .9f, .3f);
							model2.Color = iwm::vector3(.3f, .9f, .3f);

							transform1.Position.y += Time::DeltaTime();
						}
					}
				}
			}
		}*/

		for (auto entity : space.ViewComponents<Transform, Velocity>()) {
			Transform& entityTransform = entity.GetComponent<Transform>();
			Velocity& entityVelocity = entity.GetComponent<Velocity>();

			entityTransform.Position += entityVelocity.Velocity;
		}
	}

	void EntityLayer::ImGui() {
		ImGui::Begin("Entity layer");

		for (auto entity : space.ViewComponents<Transform, Camera>()) {
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
		MouseMovedEvent& event)
	{
		auto player = *space.ViewComponents<Transform, Camera>().begin();
		Transform& transform = player.GetComponent<Transform>();

		float pitch = -event.DeltaY * 0.0005f;
		float yaw   = -event.DeltaX * 0.0005f;

		iwm::quaternion deltaP = iwm::quaternion::create_from_euler_angles(pitch, 0, 0);
		iwm::quaternion deltaY = iwm::quaternion::create_from_euler_angles(0, yaw, 0);

		transform.Rotation = deltaY * transform.Rotation * deltaP;

		return false;
	}

	bool EntityLayer::On(
		MouseButtonEvent& event)
	{
		if (event.State && event.Button == IwInput::MOUSE_L_BUTTON) {
			auto player = *space.ViewComponents<Transform, Camera>().begin();
			Transform& transform = player.GetComponent<Transform>();

			iwm::vector3 pos = transform.Position + transform.Forward() * 5;
			CreateCube(pos.x, pos.y, pos.z, LoadModel("res/cube.obj", loader, device));

			space.Sort();
		}

		return false;
	}

	bool EntityLayer::On(
		KeyEvent& event)
	{
		float delta = event.State ? 5 * Time::DeltaTime() : 0;
		if (event.Button & IwInput::W | IwInput::S) {
			input.z = delta 
				* (event.InputStates[IwInput::W]
				 - event.InputStates[IwInput::S]);
		}
	
		if (event.Button == IwInput::A) {
			input.x = -delta * event.InputStates[IwInput::A];
		}

		if (event.Button == IwInput::D) {
			input.x = delta * event.InputStates[IwInput::D];
		}

		if (event.Button == IwInput::SPACE) {
			input.y = delta * event.InputStates[IwInput::SPACE];
		}

		if (event.Button == IwInput::SHIFT) {
			input.y = -delta * event.InputStates[IwInput::SHIFT];
		}

		LOG_DEBUG << input;

		return false;
	}
}
