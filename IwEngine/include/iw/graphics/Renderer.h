#pragma once

#include "IwGraphics.h"
#include "RenderTarget.h"
#include "Scene.h"
#include "Mesh.h"
#include "MeshFactory.h"
#include "iw/common/Components/Transform.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/ref.h"
#include <unordered_map>
#include <vector>

#define MAX_POINT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 4

namespace iw {
namespace Graphics {
	struct CameraData {
		matrix4 View;
		matrix4 Proj;
		matrix4 ViewProj;
		vector4 CameraPos;
	};

	struct ShadowData {
		vector4 directionalLightCount = 0; // .x
		matrix4 LightViewProj[MAX_DIRECTIONAL_LIGHTS];
	};

	// might needs pad
	struct LightData {
		vector4 LightCounts = 0; // point .x, direct .y

		struct PointLightDescription {
			vector4 Position; //vector3 Position float Radius;
			vector4 Color;
		} PointLights[MAX_POINT_LIGHTS];

		struct DirectionalLightDescription {
			vector4 InvDirection;
			vector4 Color;
		} DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
	};

	enum UBOBinding {
		CAMERA   = 0x00000001,
		SHADOWS  = 0x00000010,
		LIGHTS   = 0x00000100,
	   //MATERIAL = 0x00001000,
		ALL      = 0x11111111
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

		Mesh m_quad;

		int m_width;
		int m_height;

		// Current State of Renderer

		IUniformBuffer* m_cameraUBO;
		IUniformBuffer* m_shadowUBO;
		IUniformBuffer* m_lightUBO;

		CameraData m_cameraData;
		ShadowData m_shadowData;
		LightData  m_lightData;

		RenderState m_state;

		// state cache
		Camera*           m_camera;
		Light*            m_light;
		ref<MeshData>     m_meshData;
		ref<Shader>       m_shader;
		ref<Material>     m_material;
		ref<RenderTarget> m_target;
		float m_ambiance;
		float m_time;

		int m_debugState = -1;

		ref<RenderTarget> m_defaultTarget;

	public:
		IWGRAPHICS_API
		Renderer(
			const ref<IDevice>& device);

		IWGRAPHICS_API
		virtual ~Renderer() = default;

		IWGRAPHICS_API
		void Initialize();

		IWGRAPHICS_API
		int Width() const;

		IWGRAPHICS_API
		int Height() const;

		IWGRAPHICS_API
		void SetWidth(
			int width);

		IWGRAPHICS_API
		void SetHeight(
			int height);

		IWGRAPHICS_API
		void Resize(
			int width,
			int height);

		IWGRAPHICS_API
		void SetDefaultTarget(
			ref<RenderTarget> target);

		// Used for debudding shaders, will get passed as a uniformed named 'd_state' if not -1
		void SetDebugState(
			int state)
		{
			m_debugState = state;
			LOG_DEBUG << "Set renderer debug state to " << state;
		}

		// binds buffers to shader and initializes it. TO BE PRIVATED
		IWGRAPHICS_API
		void InitShader(
			ref<Shader>& shader,
			int bindings = 0);

		// Clears screen buffer
		IWGRAPHICS_API
		virtual void Begin(
			float time = 0.0f);

		// nothing
		IWGRAPHICS_API
		virtual void End();

		// set optional camera, identity if null
		// set optional target, screen if null
		IWGRAPHICS_API
		virtual void BeginScene(
			Camera* camera = nullptr,
			const ref<RenderTarget>& target = nullptr,
			bool clear = false);

		// calls begin scene
		// set scene lights if provided, no action if null
		IWGRAPHICS_API
		virtual void BeginScene(
			Scene* scene = nullptr,
			const ref<RenderTarget>& target = nullptr,
			bool clear = false);

		// set light camera
		// set light shader
		// set light target
		IWGRAPHICS_API
		virtual void BeginShadowCast(
			Light* light,
			bool useParticleShader = false,
			bool clear = true);

		// marks end of scene, subsequent calls to SubmitMesh will be invalid
		IWGRAPHICS_API
		virtual void EndScene();

		// marks end of shadow cast, subsequent calls to SubmitMesh will be invalid
		IWGRAPHICS_API
		virtual void EndShadowCast();

		// if rendering a scene
		//	set mesh shader
		//	set mesh material
		// if rendering a shadow map
		//	use light shader
		// set transform model matrix
		// set mesh verts and indices
		IWGRAPHICS_API
		virtual void DrawMesh(
			const Transform* transform,
			Mesh* mesh);

		// Same as DrawMesh
		IWGRAPHICS_API
		virtual void DrawMesh(
			const Transform& transform,
			Mesh& mesh);

		// Runs a shader on a fullscreen quad with verts and uvs
		IWGRAPHICS_API
		virtual void ApplyFilter(
			ref<Shader>& filter,
			const ref<RenderTarget>& source,
			const ref<RenderTarget>& target = nullptr,
			Camera* camera = nullptr);

		IWGRAPHICS_API
		/*const*/ Mesh ScreenQuad() const {
			return m_quad.MakeInstance();
		}

	//private:
		IWGRAPHICS_API
		void SetTarget(
			const ref<RenderTarget>& target,
			bool useDefault = true);

		IWGRAPHICS_API
		void SetCamera(
			Camera* camera);

		IWGRAPHICS_API
		void SetMesh(
			Mesh* mesh);

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
