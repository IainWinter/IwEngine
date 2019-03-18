#pragma once

#include "iw/engine/Layer.h"

#include "iw/graphics/Mesh.h"
#include "iw/graphics/ShaderProgram.h"

namespace IwEngine {
	class IWENGINE_API EntityLayer
		: public Layer
	{
	private:
		IwGraphics::Mesh mesh;
		IwGraphics::ShaderProgram shader;

	public:
		EntityLayer();
		~EntityLayer();

		int Initilize();
		void Destroy();
		void Update();

		bool On(WindowResizedEvent& event);
		bool On(MouseMovedEvent&    event);
		bool On(MouseButtonEvent&   event);
	};
}
