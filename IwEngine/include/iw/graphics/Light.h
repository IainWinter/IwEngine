#pragma once

#include "iw/common/coredef.h"
#include "Shader.h"
#include "Camera.h"
#include "RenderTarget.h"

namespace iw {
namespace Graphics {
	class  Renderer;

	struct Light {
	protected:
		float   m_intensity;
		glm::vec3 m_color;

		ref<RenderTarget> m_shadowTarget;
		ref<Shader>       m_shadowShader;
		ref<Shader>       m_particleShadowShader;

	public:
		IWGRAPHICS_API
		Light(
			float             intensity    = 10.0f,
			ref<RenderTarget> shadowTarget = nullptr,
			ref<Shader>       shadowShader = nullptr,
			ref<Shader>       particleShadowShader = nullptr);

		virtual ~Light() = default;

		GEN_copy(IWGRAPHICS_API, Light);
		GEN_move(IWGRAPHICS_API, Light);

		IWGRAPHICS_API
		virtual void SetupShadowCast(
			Renderer* renderer);

		IWGRAPHICS_API
		virtual void EndShadowCast(
			Renderer* renderer);

		IWGRAPHICS_API
		virtual void SetupShadowCast(
			Renderer* renderer,
			bool useParticleShader,
			bool clear)
		{
			SetupShadowCast(renderer);
		}

		IWGRAPHICS_API virtual bool CanCastShadows() const;

		IWGRAPHICS_API virtual float             Intensity()            const;
		IWGRAPHICS_API virtual glm::vec3         Color()                const;
		IWGRAPHICS_API virtual glm::vec3         Position()             const;
		IWGRAPHICS_API virtual glm::vec3         WorldPosition()        const;

		IWGRAPHICS_API virtual ref<RenderTarget> ShadowTarget()         const;
		IWGRAPHICS_API virtual ref<Shader>       ShadowShader()         const;
		IWGRAPHICS_API virtual ref<Shader>       ParticleShadowShader() const;

		IWGRAPHICS_API virtual void SetIntensity(           float              intensity);
		IWGRAPHICS_API virtual void SetColor(         const glm::vec3&         color);
		IWGRAPHICS_API virtual void SetPosition(      const glm::vec3&         position);
		IWGRAPHICS_API virtual void SetShadowTarget(        ref<RenderTarget>& shadowTarget);
		IWGRAPHICS_API virtual void SetShadowShader(        ref<Shader>&       shadowShader);
		IWGRAPHICS_API virtual void SetParticleShadowShader(ref<Shader>&       particleShadowShader);
	private:
		IWGRAPHICS_API virtual Camera* ShadowCamera() const = 0;
	};
}

	using namespace Graphics;
}
