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
		iwm::matrix4 Transformation;

		iwm::vector3 Forward() {
			return -iwm::vector3::unit_z * Rotation;
		}

		iwm::vector3 Right() {
			return iwm::vector3::unit_x * Rotation;
		}

		iwm::vector3 Up() {
			return iwm::vector3::unit_y * Rotation;
		}
	};

	struct Velocity {
		iwm::vector3 Velocity;
	};

	struct Mesh {
		IwRenderer::IVertexArray* Vertices;
		IwRenderer::IIndexBuffer* Indices;
		std::size_t Count;
	};

	struct Model {
		Mesh* Meshes;
		std::size_t MeshCount;
		iwm::vector3 Color;

		Model() = default;

		Model(
			Mesh* meshes,
			std::size_t count,
			iwm::vector3 color)
			: Meshes(meshes)
			, MeshCount(count)
			, Color(color)
		{}

		Model(
			Model&& copy) noexcept
			: Meshes(copy.Meshes)
			, MeshCount(copy.MeshCount)
			, Color(copy.Color)
		{
			copy.Meshes = nullptr;
			copy.MeshCount = 0;
			copy.Color = 0;
		}

		Model(
			const Model& copy)
			: Meshes(new Mesh[copy.MeshCount])
			, MeshCount(copy.MeshCount)
			, Color(copy.Color)
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
			Color = copy.Color;

			copy.Meshes = nullptr;
			copy.MeshCount = 0;
			copy.Color = 0;

			return *this;
		}

		Model& operator=(
			const Model& copy)
		{
			Meshes = new Mesh[copy.MeshCount];
			MeshCount = copy.MeshCount;
			Color = copy.Color;
			for (std::size_t i = 0; i < MeshCount; i++) {
				Meshes[i] = copy.Meshes[i];
			}

			return *this;
		}
	};

	struct Camera {
		iwm::matrix4 Projection;
		iwm::vector3 Rotation;
	};

	class IWENGINE_API EntityLayer
		: public Layer
	{
	private:
		IwEntity::Space space;

		Transform& playerTransform;

		IwGraphics::ModelLoader loader;

		IwRenderer::IDevice* device;
		IwRenderer::IPipeline* pipeline;

		Model cube;
		iwm::vector3 lightColor;
		float specularScale;
		int cubes = 1;

	public:
		EntityLayer();

		int  Initialize() override;
		void Destroy()    override;
		void Update()     override;
		void ImGui()      override;

		//bool On(WindowResizedEvent& event);
		bool On(MouseMovedEvent&    event);
		//bool On(MouseButtonEvent&   event);
		//bool On(MouseWheelEvent&    event);
		//bool On(KeyEvent&           event);
	private:
		void CreateCube(float x, float y, float z, Model& model);
	};
}
