#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Components/Item.h"

struct DeathTransition_System : iw::SystemBase
{
	iw::SandLayer* sand;
	float m_timeTarget;
	iw::Transform* m_coreTransform;
	iw::Rigidbody* m_coreRigidbody;
	glm::vec3 m_coreVelInitial;
	glm::vec3 m_coreScaleInitial;
	glm::vec3 m_cameraPosInitial;
	glm::vec3 m_cameraPos;

	float m_timer;
	float m_time;

	DeathTransition_System(
		iw::SandLayer* sand,
		float animationTime
	)
		: iw::SystemBase     ("Death transition")
		, sand               (sand)
					      
		, m_timeTarget       (0.2f)
		, m_coreTransform    (nullptr)
		, m_coreRigidbody    (nullptr)
		, m_coreVelInitial   (0.f)
		, m_coreScaleInitial (0.f)
		, m_cameraPosInitial (200.f, 200.f, 0.f) // 200 is the center of the plane, should just calc
		, m_cameraPos        (m_cameraPosInitial)

		, m_timer (0.f)
		, m_time  (animationTime) // total animation time
	{}

	void OnPush() override;
	void OnPop() override;
	void Update() override;
};
