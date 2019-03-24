#include "iw/engine/Entity/EntityLayer.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/log/logger.h"
#include "imgui/imgui.h"
#include "iw/util/io/File.h"

namespace IwEngine {
	EntityLayer::EntityLayer()
		: Layer("Entity")
		, viewTransform(iwm::matrix4::identity)
		, projTransform(iwm::matrix4::create_perspective_field_of_view(
			1.2f, 1.78f, 0.1f, 1000.0f))
	{}

	EntityLayer::~EntityLayer() {}

	int EntityLayer::Initilize() {
		device = new IwRenderer::GLDevice();

		IwGraphics::ModelLoader loader;
		IwGraphics::ModelData* obj = loader.Load("res/lamp.obj");

		for (size_t i = 0; i < obj->MeshCount; i++) {
			IwRenderer::IndexBuffer* ib = device->CreateIndexBuffer(
				obj->Indices[i].FaceCount,
				obj->Indices[i].Faces);

			IwRenderer::VertexBufferLayout* vbl
				= new IwRenderer::VertexBufferLayout();
			vbl->Push<float>(3);
			vbl->Push<float>(3);

			IwRenderer::VertexBuffer* vb = device->CreateVertexBuffer(
				obj->Meshes[i].VertexCount * sizeof(IwGraphics::Vertex),
				obj->Meshes[i].Vertices);

			IwRenderer::VertexArray* va
				= device->CreateVertexArray(1, &vb, &vbl);

			model.push_back({ va, ib, obj->Indices[i].FaceCount });
		}

		IwRenderer::VertexShader* vs = device->CreateVertexShader(
			IwUtil::ReadFile("res/defaultvs.glsl").c_str());

		IwRenderer::FragmentShader* fs = device->CreateFragmentShader(
			IwUtil::ReadFile("res/defaultfs.glsl").c_str());

		pipeline = device->CreatePipeline(vs, fs);

		pos = { 0, 0, -10 };
		vel = { 0, 0, 0 };
		rot = 0;

		return 0;
	}

	void EntityLayer::Destroy() {
		for (auto& mesh : model) {
			device->DestroyVertexArray(mesh.Vertices);
			device->DestroyIndexBuffer(mesh.Indices);
		}
	}

	void EntityLayer::Update() {
		modelTransform = iwm::matrix4::create_rotation_y(rot)
			* iwm::matrix4::create_translation(pos.x - 5, pos.y, pos.z);

		device->SetPipeline(pipeline);

		IwRenderer::PipelineParam* umodel = pipeline->GetParam("model");
		IwRenderer::PipelineParam* uview = pipeline->GetParam("view");
		IwRenderer::PipelineParam* uproj = pipeline->GetParam("proj");

		umodel->SetAsMat4(modelTransform);
		uview ->SetAsMat4(viewTransform);
		uproj ->SetAsMat4(projTransform);

		for (auto& mesh : model) {
			device->SetVertexArray(mesh.Vertices);
			device->SetIndexBuffer(mesh.Indices);
			device->DrawElements(mesh.Count, 0);
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
