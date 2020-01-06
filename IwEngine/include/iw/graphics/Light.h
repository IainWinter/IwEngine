#pragma once

#include "Shader.h"
#include "Camera.h"
#include "RenderTarget.h"

namespace IW {
namespace Graphics {
	struct Light {
	private:
		iw::ref<Shader> m_lightShader;
		iw::ref<RenderTarget> m_lightTarget;

		// Filter
		//iw::ref<Shader> m_null;
		//iw::ref<Shader> m_post;
		//iw::ref<IW::RenderTarget> m_postTarget;

	public:
		IWGRAPHICS_API
		Light() = default;

		IWGRAPHICS_API
		Light(
			iw::ref<Shader> shader,
			iw::ref<RenderTarget> target);

		IWGRAPHICS_API
		const iw::ref<Shader>& LightShader() const;

		IWGRAPHICS_API
		const iw::ref<RenderTarget>& LightTarget() const;

		IWGRAPHICS_API
		virtual Camera& Cam() = 0;

		IWGRAPHICS_API
		virtual void SetPosition(
			const iw::vector3& position) = 0;

		IWGRAPHICS_API
		virtual void SetRotation(
			const iw::quaternion& rotation) = 0;

		//IWGRAPHICS_API
		//virtual void PostProcess();

		//IWGRAPHICS_API
		//virtual const Camera& Cam() const = 0;

		//IWGRAPHICS_API
		//virtual Camera& Cam() = 0;

		//IWGRAPHICS_API
		//const iw::ref<Shader>& NullFilter() const;

		//IWGRAPHICS_API
		//const iw::ref<Shader>& PostFilter() const;

		//IWGRAPHICS_API
		//const iw::ref<RenderTarget>& PostTarget() const;

		//IWGRAPHICS_API
		//void SetLightShader(
		//	iw::ref<Shader>& lightShader);

		//IWGRAPHICS_API
		//void SetNullFilter(
		//	iw::ref<Shader>& nullFilter);

		//IWGRAPHICS_API
		//void SetShadowTarget(
		//	iw::ref<RenderTarget>& shadowTarget);

		//IWGRAPHICS_API
		//void SetPostFilter(
		//	iw::ref<Shader>& postFilter);

		//IWGRAPHICS_API
		//void SetPostTarget(
		//	iw::ref<RenderTarget>& postTarget);
	};
}

	using namespace Graphics;
}
