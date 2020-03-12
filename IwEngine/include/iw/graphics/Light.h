#pragma once

#include "Shader.h"
#include "Camera.h"
#include "RenderTarget.h"

namespace iw {
namespace Graphics {
	class Renderer;

	struct Light {
	protected:
		float m_intensity;

		// optional for shadow mapping

		Camera*           m_shadowCamera;
		ref<Shader>       m_shadowShader;
		ref<RenderTarget> m_shadowTarget;

	public:
		// Takes ownership of camera ptr
		IWGRAPHICS_API
		Light(
			float             intensity    = 10.0f,
			Camera*           shadowCamera = nullptr,
			ref<Shader>       shadowShader = nullptr,
			ref<RenderTarget> shadowTarget = nullptr);

		IWGRAPHICS_API
		virtual ~Light();

		IWGRAPHICS_API
		virtual void SetupShadowCast(
			Renderer* renderer) = 0;

		IWGRAPHICS_API
		bool CanCastShadows() const;

		IWGRAPHICS_API       float              Intensity()    const;
		IWGRAPHICS_API       float&             Intensity();

		IWGRAPHICS_API const vector3&           Position()     const;
		IWGRAPHICS_API       vector3&           Position();

		IWGRAPHICS_API const ref<Shader>&       ShadowShader() const;
		IWGRAPHICS_API       ref<Shader>&       ShadowShader();

		IWGRAPHICS_API const ref<RenderTarget>& ShadowTarget() const;
		IWGRAPHICS_API       ref<RenderTarget>& ShadowTarget();

		IWGRAPHICS_API
		void SetIntensity(
			float intensity);

		IWGRAPHICS_API
		void SetPosition(
			const vector3& position);

		//IWGRAPHICS_API
		//void SetLightShader(
		//	ref<Shader>& shadowShader);

		//IWGRAPHICS_API
		//void SetShadowTarget(
		//	ref<RenderTarget>& shadowTarget);


		//IWGRAPHICS_API
		//virtual Camera& Cam() = 0;
		//
		//IWGRAPHICS_API
		//virtual void SetRotation(
		//	const quaternion& rotation) = 0;
		//
		//IWGRAPHICS_API
		//virtual void PostProcess();
		//
		//IWGRAPHICS_API
		//virtual const Camera& Cam() const = 0;
		//
		//IWGRAPHICS_API
		//virtual Camera& Cam() = 0;
		//
		//IWGRAPHICS_API
		//const ref<Shader>& NullFilter() const;
		//
		//IWGRAPHICS_API
		//const ref<Shader>& PostFilter() const;
		//
		//IWGRAPHICS_API
		//const ref<RenderTarget>& PostTarget() const;
		//
		//IWGRAPHICS_API
		//void SetLightShader(
		//	ref<Shader>& lightShader);
		//
		//IWGRAPHICS_API
		//void SetNullFilter(
		//	ref<Shader>& nullFilter);
		//
		//IWGRAPHICS_API
		//void SetShadowTarget(
		//	ref<RenderTarget>& shadowTarget);
		//
		//IWGRAPHICS_API
		//void SetPostFilter(
		//	ref<Shader>& postFilter);
		//
		//IWGRAPHICS_API
		//void SetPostTarget(
		//	ref<RenderTarget>& postTarget);
	};
}

	using namespace Graphics;
}
