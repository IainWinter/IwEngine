#pragma once

#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"

#include "iw/graphics/Font.h"
#include "Components/Score.h"
#include "iw/graphics/Camera.h"
#include "iw/graphics/Mesh.h"
#include <unordered_map>

class ScoreSystem
	: public iw::System<iw::Transform, Score>
{
public:
	struct Components {
		iw::Transform* Transform;
		Score* Score;
	};

	iw::Camera* camera;
	iw::Camera* uiCam;

	iw::Mesh* totalScoreMesh;
	int totalScore;

	iw::ref<iw::Font> font;
	iw::ref<iw::Material> textMat;
	std::unordered_map<int, iw::ref<iw::Mesh>> scores;

public:
	ScoreSystem(
		iw::Camera* camera,
		iw::Camera* uiCam);

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	bool On(iw::CollisionEvent& e) override;

private:
	void SpawnScore(
		int score,
		iw::vector3 position);
};
