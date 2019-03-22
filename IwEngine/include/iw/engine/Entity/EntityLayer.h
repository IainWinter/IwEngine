#pragma once

#include "iw/engine/Layer.h"

#include "iw/renderer/Mesh.h"
#include "iw/renderer/Renderer.h"

namespace IwEngine {
	class IWENGINE_API EntityLayer
		: public Layer
	{
	private:
		iwm::vector3 pos;
		iwm::vector3 vel;
		float rot;
		std::vector<IwRenderer::Mesh> model;
		IwRenderer::ShaderProgram* shader;
		IwRenderer::Renderer renderer;
		iwm::matrix4 camera;

	public:
		EntityLayer();
		~EntityLayer();

		int Initilize() override;
		void Destroy() override;
		void Update() override;
		void ImGui() override;

		bool On(WindowResizedEvent& event);
		bool On(MouseMovedEvent&    event);
		bool On(MouseButtonEvent&  event);
		bool On(MouseWheelEvent&   event);
	};
}
