#pragma once

#include "iw/engine/Layer.h"

#include "iw/graphics/Mesh.h"
#include "iw/graphics/ShaderProgram.h"

namespace IwEngine {
	class IWENGINE_API EntityLayer
		: public Layer
	{
	private:
		iwm::vector3 pos;
		iwm::vector3 vel;
		float rot;
		std::vector<IwGraphics::Mesh> model;
		IwGraphics::ShaderProgram* shader;

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
