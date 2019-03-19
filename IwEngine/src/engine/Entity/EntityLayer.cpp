#include "iw/engine/Entity/EntityLayer.h"
#include "iw/engine/Resources/Loaders/ObjLoader.h" 
#include "imgui/imgui.h"

namespace IwEngine {
	EntityLayer::EntityLayer()
		: Layer("Entity")
	{}

	EntityLayer::~EntityLayer() {
	}

	int EntityLayer::Initilize() {
		Obj object = IwUtil::Load<Obj>("res/test.obj");

		IwGraphics::VertexBufferLayout* vbl 
			= new IwGraphics::VertexBufferLayout();
		vbl->Push<float>(3);
		vbl->Push<float>(2);
		vbl->Push<float>(3);

		IwGraphics::VertexArray* va = new IwGraphics::VertexArray();
		IwGraphics::IndexBuffer* ib = new IwGraphics::IndexBuffer(
			object.Faces, 72);

		IwGraphics::VertexBuffer* vbv = new IwGraphics::VertexBuffer(
			object.Vertices, sizeof(iwm::vector3) * 8);

		IwGraphics::VertexBuffer* vbu = new IwGraphics::VertexBuffer(
			object.Uvs, sizeof(iwm::vector2) * 5);

		IwGraphics::VertexBuffer* vbn = new IwGraphics::VertexBuffer(
			object.Normals, sizeof(iwm::vector3) * 6);

		va->AddBuffer(vbv, vbl);
		va->AddBuffer(vbu, vbl);
		va->AddBuffer(vbn, vbl);

		mesh = IwGraphics::Mesh(va, ib);
		shader = new IwGraphics::ShaderProgram("res/default.shader");
		
		pos = { -.5f, -.5f, -3 };
		vel = { 0, 0, 0 };
		rot = 0;

		return 0;
	}

	void EntityLayer::Destroy() {
	}

	void EntityLayer::Update() {
		shader->Use();
		mesh.Draw(pos, iwm::quaternion::create_from_euler_angles(0, rot, 0));

		pos += vel;
	}

	void EntityLayer::ImGui() {
		ImGui::Begin("Entity layer");

		ImGui::Text("Pos (x, y, z): %f %f %f", pos.x, pos.y, pos.z);
		ImGui::Text("Rot (y): %f", rot);

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
		float speed = event.State ? .2f : 0.0f;
		if (event.Button == IwInput::MOUSE_L_BUTTON) {
			vel.z = -speed;
		}

		else if (event.Button == IwInput::MOUSE_R_BUTTON) {
			vel.z = speed;
		}

		else if (event.Button == IwInput::MOUSE_X1_BUTTON) {
			vel.x = speed;
		}

		else if (event.Button == IwInput::MOUSE_X2_BUTTON) {
			vel.x = -speed;
		}

		return false;
	}

	bool EntityLayer::On(
		MouseWheelEvent& event) 
	{
		rot += event.Delta * .1f;

		return false;
	}
}
