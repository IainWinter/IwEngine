#pragma once

#include "iw/engine/Layer.h"

#include "iw/entity/Space.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Pipeline.h"

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

	struct Model {
		Mesh* Meshes;
		std::size_t MeshCount;

		Model(
			Mesh* meshes,
			std::size_t count)
			: Meshes(meshes)
			, MeshCount(count)
		{}

		Model(
			Model&& copy) noexcept
			: Meshes(copy.Meshes)
			, MeshCount(copy.MeshCount)
		{
			copy.Meshes = nullptr;
			copy.MeshCount = 0;
		}

		Model(
			const Model& copy)
			: Meshes(new Mesh[copy.MeshCount])
			, MeshCount(copy.MeshCount)
		{
			for (std::size_t i = 0; i < MeshCount; i++) {
				Meshes[i] = copy.Meshes[i];
			}
		}

		~Model() {
			delete Meshes;
		}

		Model& operator=(
			Model&& copy) noexcept
		{
			Meshes = copy.Meshes;
			MeshCount = copy.MeshCount;

			copy.Meshes = nullptr;
			copy.MeshCount = 0;

			return *this;
		}

		Model& operator=(
			const Model& copy)
		{
			Meshes = new Mesh[copy.MeshCount];
			MeshCount = copy.MeshCount;
			for (std::size_t i = 0; i < MeshCount; i++) {
				Meshes[i] = copy.Meshes[i];
			}

			return *this;
		}
	};

	class IWENGINE_API EntityLayer
		: public Layer
	{
	private:
		IwEntity::Space space;

		IwGraphics::ModelLoader loader;

		IwRenderer::IDevice* device;
		IwRenderer::IPipeline* pipeline;

		iwm::vector3 lightColor;
		float lightAngle;
		float specularScale;
		iwm::matrix4 viewTransform;
		iwm::matrix4 projTransform;

	public:
		EntityLayer();
		~EntityLayer();

		int  Initialize() override;
		void Destroy()    override;
		void Update()     override;
		void ImGui()      override;

		bool On(WindowResizedEvent& event);
		bool On(MouseMovedEvent&    event);
		bool On(MouseButtonEvent&   event);
		bool On(MouseWheelEvent&    event);
	private:
		void CreateCube(float x, float y);
	};
}
