#pragma once

#include "iw/engine/Layer.h"

#include "iw/renderer/Device.h"
#include "iw/renderer/Pipeline.h"
#include "iw/entity/Space.h"

namespace IwEngine {
	struct Transform {
		iwm::vector3 Position;
		iwm::quaternion Rotation;

		iwm::matrix4 GetTransformation() {
			return iwm::matrix4::create_from_quaternion(Rotation)
				* iwm::matrix4::create_translation(Position);
		}
	};

	struct Velocity {
		iwm::vector3 Velocity;
	};

	struct Mesh {
		IwRenderer::IVertexArray* Vertices;
		IwRenderer::IIndexBuffer* Indices;
		unsigned int Count;
	};

	class IWENGINE_API EntityLayer
		: public Layer
	{
	private:
		IwEntity::Space space;
		IwEntity::Entity model;
		IwRenderer::IDevice* device;
		IwRenderer::IPipeline* pipeline;

		iwm::vector3 lightColor;
		float lightAngle;
		float specularScale;
		std::vector<Mesh> modelMeshes;
		iwm::matrix4 viewTransform;
		iwm::matrix4 projTransform;

	public:
		EntityLayer();
		~EntityLayer();

		int  Initialize() override;
		void Destroy()   override;
		void Update()    override;
		void ImGui()     override;

		bool On(WindowResizedEvent& event);
		bool On(MouseMovedEvent&    event);
		bool On(MouseButtonEvent&  event);
		bool On(MouseWheelEvent&   event);
	};
}
