#pragma once

#include "IwGraphics.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "Light.h"
#include "iw/renderer/Device.h"
#include "iw/common/Components/Transform.h"
#include "iw/util/memory/ref.h"
#include <unordered_map>
#include <vector>

#include "iw/graphics/PointLight.h"
#include "iw/graphics/DirectionalLight.h"

#define MAX_POINT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 4

namespace iw {
namespace Graphics {
	struct Scene {
		Camera* Camera;
		std::vector<PointLight*> PointLights;
		std::vector<DirectionalLight*> DirectionalLights;
	};

	struct CameraData {
		matrix4 ViewProj;
		vector4 CameraPos;
	};

	struct LightData {
		vector2 PAD;
		float PointLightCount;
		float DirectionalLightCount;

		struct PointLightDescription {
			vector3 Position;
			float Radius;
		} PointLights[MAX_POINT_LIGHTS];

		struct DirectionalLightDescription {
			vector3 Position;
			vector3 Direction;
		} DirectionalLights[MAX_POINT_LIGHTS];
	};

	enum UBOBinding {
		CAMERA   = 0x001,
		LIGHTS   = 0x010,
	   //MATERIAL = 0x100,
		ALL      = 0x111
	};

	class Renderer {
	public:
		ref<IDevice> Device;
	private:
		enum class RenderState {
			SCENE,
			SHADOW_MAP,
			INVALID
		};

		const Mesh* m_filterMesh;

		int m_width;
		int m_height;

		// Current State of Renderer

		IUniformBuffer* m_cameraUBO;
		IUniformBuffer* m_lightUBO;

		CameraData m_cameraData;
		LightData  m_lightData;

		ref<Shader>   m_shader;
		ref<Material> m_material;
		ref<RenderTarget> m_target;

		RenderState m_state;

	public:
		IWGRAPHICS_API
		Renderer(
			const ref<IDevice>& device);

		IWGRAPHICS_API
		~Renderer();

		IWGRAPHICS_API
		void Initialize();

		IWGRAPHICS_API
		int Width() const;

		IWGRAPHICS_API
		int Height() const;

		IWGRAPHICS_API
		void Resize(
			int width,
			int height);

		// Clears screen buffer
		IWGRAPHICS_API
		void Begin();

		// executes queue
		IWGRAPHICS_API
		void End();

		// binds buffers to shader and initializes it. TO BE PRIVATED
		IWGRAPHICS_API
		void InitShader(
			ref<Shader>& shader,
			int bindings);

		// set optional camera, identity if null
		// set optional target, screen if null
		IWGRAPHICS_API
		void BeginScene(
			Camera* camera = nullptr,
			const ref<RenderTarget>& target = nullptr);

		// calls begin scene
		// set scene lights if provided, no action if null
		IWGRAPHICS_API
		void BeginScene(
			Scene* scene = nullptr,
			const ref<RenderTarget>& target = nullptr);

		// marks end of scene, subsequent calls to SubmitMesh will be invalid
		IWGRAPHICS_API
		void EndScene();

		// set light camera
		// set light shader
		// set light target
		IWGRAPHICS_API
		void BeginShadowCast(
			Light* light);

		// marks end of shadow cast, subsequent calls to SubmitMesh will be invalid
		IWGRAPHICS_API
		void EndShadowCast(
			const Light* light);

		// if rendering a scene
		//	set mesh shader
		//	set mesh material
		// if rendering a shadow map
		//	use light shader
		// set transform model matrix
		// set mesh verts and indices
		IWGRAPHICS_API
		void DrawMesh(
			const Transform* transform,
			const Mesh* mesh);

		// tbd
		IWGRAPHICS_API
		void ApplyFilter(
			ref<Shader>& filter,
			const ref<RenderTarget>& source,
			const ref<RenderTarget>& destination = nullptr);

	//private:
		IWGRAPHICS_API
		void SetTarget(
			const ref<RenderTarget>& target);

		IWGRAPHICS_API
		void SetCamera(
			Camera* camera);

		IWGRAPHICS_API
		void SetShader(
			const ref<Shader>& shader);

		IWGRAPHICS_API
		void SetMaterial(
			const ref<Material>& material);

		IWGRAPHICS_API
		void SetPointLights(
			const std::vector<PointLight*>& lights);

		IWGRAPHICS_API
		void SetDirectionalLights(
			const std::vector<DirectionalLight*>& lights);
	};
}

	using namespace Graphics;
}
