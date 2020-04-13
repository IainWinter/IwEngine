#pragma once

#include "Components/Bullet.h"
#include "iw/engine/System.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/common/Components/Transform.h"

#include "iw/graphics/Font.h"

#include "Components/Score.h"

class ScoreSystem
	: public iw::System<iw::Transform, iw::Font, Score>
{
public:
	struct Components {
		iw::Transform* Transform;
		iw::Font* Font;
		Score* Score;
	};

public:
	ScoreSystem();

	int Initialize() override;

	void FixedUpdate(
		iw::EntityComponentArray& view) override;

	bool On(iw::CollisionEvent& e) override;
};
