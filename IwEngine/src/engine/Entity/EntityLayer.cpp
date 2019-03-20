#include "iw/engine/Entity/EntityLayer.h"
#include "iw/engine/Resources/Loaders/ObjectLoader.h" 
#include "iw/log/logger.h"
#include "imgui/imgui.h"

#include "tinyobjloader/tiny_obj_loader.h"

namespace IwEngine {
	EntityLayer::EntityLayer()
		: Layer("Entity")
	{}

	EntityLayer::~EntityLayer() {
	}

	int EntityLayer::Initilize() {
		ObjLoader loader;
		Object* obj = loader.Load("res/Bear.obj");

		IwGraphics::VertexArray* va = new IwGraphics::VertexArray();

		IwGraphics::IndexBuffer* ib = new IwGraphics::IndexBuffer(
			&obj->Faces[0], obj->Faces.size());

		IwGraphics::VertexBufferLayout* vbl
			= new IwGraphics::VertexBufferLayout();

		vbl->Push<float>(3);

		IwGraphics::VertexBuffer* vb = new IwGraphics::VertexBuffer(
			&obj->Vertices[0], sizeof(iwm::vector3) * obj->Vertices.size());

		va->AddBuffer(vb, vbl);

		mesh = IwGraphics::Mesh(va, ib);
		shader = new IwGraphics::ShaderProgram("res/default.shader");
		
		pos = { -.5f, -.5f, -5 };
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
