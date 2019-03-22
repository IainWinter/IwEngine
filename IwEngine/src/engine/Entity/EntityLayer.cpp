#include "iw/engine/Entity/EntityLayer.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/log/logger.h"
#include "imgui/imgui.h"

namespace IwEngine {
	EntityLayer::EntityLayer()
		: Layer("Entity")
	{}

	EntityLayer::~EntityLayer() {
	}

	int EntityLayer::Initilize() {
		camera = iwmath::matrix4::create_rotation(0, rot, 0)
			* iwmath::matrix4::create_translation(pos);

		IwGraphics::ModelLoader loader;

		IwGraphics::ModelData* obj = loader.Load("res/lamp.obj");

		for (size_t i = 0; i < obj->MeshCount; i++) {
			IwRenderer::VertexArray* va = new IwRenderer::VertexArray();

			IwRenderer::IndexBuffer* ib = new IwRenderer::IndexBuffer(
				obj->Indices[i].Faces, obj->Indices[i].FaceCount);
		
			IwRenderer::VertexBufferLayout* vbl
				= new IwRenderer::VertexBufferLayout();
			vbl->Push<float>(3);
			vbl->Push<float>(3);

			IwRenderer::VertexBuffer* vbv = new IwRenderer::VertexBuffer(
				obj->Meshes[i].Vertices, 
				sizeof(IwGraphics::Vertex) * obj->Meshes[i].VertexCount);

			va->AddBuffer(vbv, vbl);

			model.push_back(IwRenderer::Mesh(va, ib));
		}

		shader = new IwRenderer::ShaderProgram("res/default.shader");
		
		pos = { 0, 0, -10 };
		vel = { 0, 0, 0 };
		rot = 0;

		return 0;
	}

	void EntityLayer::Destroy() {
	}

	void EntityLayer::Update() {
		for (auto& mesh : model) {
			IwRenderer::DrawItem draw;
			draw.State.Mesh = &mesh;
			draw.State.ShaderProgram = shader;
			draw.State.Transformation = &camera;

			renderer.Submit(draw);
		}

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
