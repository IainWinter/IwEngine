#include "iw/engine/Entity/EntityLayer.h"

namespace IwEngine {
	EntityLayer::EntityLayer()
		: Layer("Entity")
	{}

	EntityLayer::~EntityLayer() {
	}

	int EntityLayer::Initilize() {
		float* points = new float[6]{
		 0.1f, -0.1f,
		 0.0f,  0.1f,
		-0.1f, -0.1f
		};

		unsigned int* index = new unsigned int[3]{
			0, 1, 2
		};

		shader = IwGraphics::ShaderProgram("res/default.shader");

		IwGraphics::VertexBufferLayout* vbl = new IwGraphics::VertexBufferLayout();
		vbl->Push<float>(2);

		IwGraphics::VertexArray* va = new IwGraphics::VertexArray();
		IwGraphics::IndexBuffer* ib = new IwGraphics::IndexBuffer(index, 3);

		IwGraphics::VertexBuffer* vb = new IwGraphics::VertexBuffer(points, sizeof(float) * 6);
		va->AddBuffer(vb, vbl);

		mesh = IwGraphics::Mesh(va, ib);

		return 0;
	}

	void EntityLayer::Destroy() {
	}

	void EntityLayer::Update() {
		shader.Use();

		iwmath::vector3 pos = iwmath::vector3(.1f, .2f, 0);
		iwmath::quaternion rot = iwmath::quaternion::create_from_euler_angles(0, 0, 30);
		mesh.Draw(pos, rot);
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
		return false;
	}
}
