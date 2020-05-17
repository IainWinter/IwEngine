#pragma once

#include "Shader.h"
#include "Camera.h"
#include "RenderTarget.h"

namespace iw {
namespace Graphics {
	class Renderer;

	struct Light { // need copy constructor
	protected:
		float m_intensity;

		// optional for shadow mapping

		Camera*           m_shadowCamera;
		ref<RenderTarget> m_shadowTarget;
		ref<Shader>       m_shadowShader;
		ref<Shader>       m_particleShadowShader;

	public:
		// Takes ownership of camera ptr
		IWGRAPHICS_API
		Light(
			float             intensity    = 10.0f,
			Camera*           shadowCamera = nullptr,
			ref<RenderTarget> shadowTarget = nullptr,
			ref<Shader>       shadowShader = nullptr,
			ref<Shader>       particleShadowShader = nullptr);

		IWGRAPHICS_API
		virtual ~Light();

		IWGRAPHICS_API
		virtual void SetupShadowCast(
			Renderer* renderer) = 0;

		IWGRAPHICS_API
		virtual void EndShadowCast(
			Renderer* renderer);

		IWGRAPHICS_API
		virtual bool CanCastShadows() const;

		IWGRAPHICS_API
		virtual bool Outdated() const;

		IWGRAPHICS_API       float              Intensity()    const;
		IWGRAPHICS_API const vector3&           Position()     const;
		IWGRAPHICS_API const ref<RenderTarget>& ShadowTarget() const;
		IWGRAPHICS_API const ref<Shader>&       ShadowShader() const;
		IWGRAPHICS_API const ref<Shader>&       ParticleShadowShader() const;
		IWGRAPHICS_API const Camera*            ShadowCamera() const;

		IWGRAPHICS_API
		void SetIntensity(
			float intensity);

		IWGRAPHICS_API
		void SetPosition(
			const vector3& position);

		IWGRAPHICS_API
		virtual void SetShadowTarget(
			ref<RenderTarget>& shadowTarget);

		IWGRAPHICS_API
		virtual void SetShadowShader(
			ref<Shader>& shadowShader);
		
		IWGRAPHICS_API
		virtual void SetParticleShadowShader(
			ref<Shader>& particleShadowShader);

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
