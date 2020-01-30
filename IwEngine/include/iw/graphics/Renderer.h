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

namespace IW {
namespace Graphics {
	//class RenderTargetBuilder {
	//private:
	//	int m_width;
	//	int m_height;
	//	std::vector<iw::ref<Texture>> m_textures;
	//	iw::ref<IDevice> m_device;

	//public:
	//	RenderTargetBuilder(
	//		iw::ref<IDevice> device)
	//		: m_device(m_device)
	//	{}

	//	RenderTargetBuilder& SetWidth(
	//		int width)
	//	{
	//		m_width = width;
	//	}

	//	RenderTargetBuilder& SetHeight(
	//		int height)
	//	{
	//		m_height = height;
	//	}

	//	RenderTargetBuilder& AddTexture(
	//		iw::ref<Texture> texture)
	//	{
	//		m_textures.push_back(texture);
	//	}

	//	iw::ref<RenderTarget> Initialize() {
	//		auto freee = iw::make_callback(&IDevice::DestroyFrameBuffer, m_device.get());

	//		iw::ref<RenderTarget> rt = iw::ref<RenderTarget>(new RenderTarget(m_width, m_height), freee);
	//		for (iw::ref<Texture>& tex : m_textures) {
	//			rt->AddTexture(tex);
	//		}

	//		rt->Initialize(m_device);

	//		return rt;
	//	}
	//};

	struct CameraData {
		iw::matrix4 ViewProj;
	};

	enum UBOBinding {
		CAMERA   = 0x01,
		//MATERIAL = 0x10,
		ALL      = 0x11
	};

	class Renderer {
	public:
		//struct SceneData {
		//	iw::matrix4 World;
		//	iw::ref<Camera> Camera;
		//	std::vector<Light> Lights;
		//	RenderTarget FinalTarget;
		//	//RenderTarget FinalPostProcessTarget;
		//};

		int Width;
		int Height;

		iw::ref<IDevice> Device;
	private:
		Mesh* m_filterMesh;

		IUniformBuffer* m_cameraUBO;
		CameraData m_cameraData;

	public:
		IWGRAPHICS_API
		Renderer(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		~Renderer();

		IWGRAPHICS_API
		void Initialize();

		IWGRAPHICS_API
		void Begin();

		IWGRAPHICS_API
		void End();

		IWGRAPHICS_API
		void InitShader(
			iw::ref<Shader>& shader,
			UBOBinding bindings);

		IWGRAPHICS_API
		void SetShader(
			const iw::ref<Shader>& shader);

		IWGRAPHICS_API
		void SetCamera(
			Camera* camera);

		IWGRAPHICS_API
		void BeginScene(
			Camera* camera = nullptr,
			RenderTarget* target = nullptr);

		IWGRAPHICS_API
		void EndScene();

		IWGRAPHICS_API
		void DrawMesh(
			const Transform* transform,
			const Mesh* mesh);

		IWGRAPHICS_API
		void BeginLight(
			Light* light);

		IWGRAPHICS_API
		void EndLight(
			const Light* light);

		IWGRAPHICS_API
		void CastMesh(
			const Light* light,
			const Transform* transform,
			const Mesh* mesh);

		IWGRAPHICS_API
		void ApplyFilter(
			iw::ref<Shader>& filter,
			RenderTarget* source,
			RenderTarget* dest = nullptr);

		// I kinda like this idea

		//IWGRAPHICS_API
		//RenderTargetBuilder BuildRenderTarget() {
		//	return RenderTargetBuilder(Device);
		//}
	};
}

	using namespace Graphics;
}
