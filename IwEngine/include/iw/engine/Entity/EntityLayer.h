#pragma once

#include "iw/engine/Layer.h"

#include "iw/graphics/Mesh.h"
#include "iw/graphics/ShaderProgram.h"
#include "tinyobjloader/tiny_obj_loader.h"

namespace IwEngine {
	class IWENGINE_API EntityLayer
		: public Layer
	{
	private:
		iwm::vector3 pos;
		iwm::vector3 vel;
		float rot;
		IwGraphics::Mesh mesh;
		IwGraphics::ShaderProgram* shader;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::vector<unsigned int> vertIndex;
		std::vector<float> vertBuffer;

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
