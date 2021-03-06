#include "Systems/ScoreSystem.h"
#include "Events/ActionEvents.h"
#include "Components/LevelDoor.h"
#include "Components/Player.h"
#include "Components/Enemy.h"
#include "iw/engine/Time.h"
#include "Components/EnemyDeathCircle.h"

#include "iw/engine/Components/UiElement_temp.h"

ScoreSystem::ScoreSystem(
	iw::Entity& player,
	iw::Camera* camera,
	iw::Camera* uiCam)
	: iw::System<iw::Transform, Score>("Score")
	, player(player)
	, totalScore(0.0f)
	, potentialScore(0.0f)
	, camera(camera)
	, uiCam(uiCam)
{}

int ScoreSystem::Initialize() {
	// Fonts

	font = Asset->Load<iw::Font>("fonts/arial.fnt");
	//font->Initialize(Renderer->Device);

	iw::ref<iw::Shader> fontShader = Asset->Load<iw::Shader>("shaders/font_simple.shader");

	textMatBad = REF<iw::Material>(fontShader);
	textMat    = REF<iw::Material>(fontShader);

	textMatBad->Set("color", iw::Color(1, 0.1f, 0.1f));
	textMatBad->SetTexture("fontMap", font->GetTexture(0));
	textMatBad->SetTransparency(iw::Transparency::ADD);

	textMat   ->Set("color", iw::Color(1));
	textMat   ->SetTexture("fontMap", font->GetTexture(0));
	textMat   ->SetTransparency(iw::Transparency::ADD);

	textMatBad->Initialize(Renderer->Device);
	textMat   ->Initialize(Renderer->Device);

	totalScoreMesh     = font->GenerateMesh(std::to_string(totalScore), .02f, 1);
	potentialScoreMesh = font->GenerateMesh(std::to_string(potentialScore), .02f, 1);
	
	potentialScoreMesh.SetMaterial(textMat);
	totalScoreMesh    .SetMaterial(textMat);

	return  0;
}

void ScoreSystem::Update(
	iw::EntityComponentArray& view)
{
	Renderer->BeginScene(camera);

		for (auto entity : view) {
			auto [transform, score] = entity.Components.Tie<Components>();

			auto itr = scores.find(score->Score);
			if (itr == scores.end()) {
				itr = scores.emplace(score->Score, font->GenerateMesh(std::to_string(score->Score), .011f, 1)).first;

				if (score->Score < 0) {
					itr->second.SetMaterial(textMatBad);
				}

				else if (score->Score >= 0) {
					itr->second.SetMaterial(textMat);
				}

				itr->second.Data()->Initialize(Renderer->Device);
			}

			iw::Transform trans = *transform;
			trans.Position.y +=  0.1f * (score->Timer / score->Lifetime);
			trans.Position.z += -2    * (score->Timer / score->Lifetime);

			trans.Rotation = iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2);

			trans.Scale = 1.0f;

			if (score->Timer >= score->Lifetime) {
				Space->QueueEntity(entity.Handle, iw::func_Destroy);
			}

			score->Timer += iw::Time::DeltaTime();

			Renderer->DrawMesh(trans, itr->second);
		}

	Renderer->EndScene();

	font->UpdateMesh(totalScoreMesh, std::to_string(totalScore), .01f, 1);
	//totalScoreMesh.Data()->Update(Renderer->Device);

	font->UpdateMesh(potentialScoreMesh, std::to_string(potentialScore), .01f, 1);
	//potentialScoreMesh.Data()->Update(Renderer->Device);

	iw::Transform t;
	iw::Transform tp;
	t.Position  = iw::vector3(-7.75, 4.5,  -5);
	tp.Position = iw::vector3(-7.75, 3.75, -5);

	if (totalScore < 0) {
		totalScoreMesh.SetMaterial(textMatBad);
	}

	else if (totalScore >= 0) {
		totalScoreMesh.SetMaterial(textMat);
	}

	if (potentialScore < 0) {
		potentialScoreMesh.SetMaterial(textMatBad);
	}

	else if (potentialScore >= 0) {
		potentialScoreMesh.SetMaterial(textMat);
	}

	// Temp should be ui element

	//Renderer->BeforeScene([&]() {
	//	Renderer->Device->SetDepthTest(false);
	//});

	//Renderer->AfterScene([&]() {
	//	Renderer->Device->SetDepthTest(true);
	//});

	Renderer->BeginScene(uiCam);

		Renderer->DrawMesh(t,  totalScoreMesh);
		Renderer->DrawMesh(tp, potentialScoreMesh);

	Renderer->EndScene();
}

bool ScoreSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::GIVE_SCORE): {
			GiveScoreEvent& event = e.as<GiveScoreEvent>();

			if (event.HalfScore) {
				int score = potentialScore / 2;
				score = score - (score % 50);

				SpawnScore(-score, event.Position);
			}

			else {
				SpawnScore(event.Score, event.Position);
			}

			break;
		}
		case iw::val(Actions::ACTIVATE_LEVEL): {
			totalScore += potentialScore;
			potentialScore = 0;
			break;
		}
		case iw::val(Actions::RESET_LEVEL): {
			potentialScore = 0;
			break;
		}
	}

	return false;
}

void ScoreSystem::SpawnScore(
	int score, 
	iw::vector3 position)
{
	if (score == 0) {
		return;
	}

	iw::Entity entity = Space->CreateEntity<iw::Transform, Score>();

	entity.Set<iw::Transform>(position);
	entity.Set<Score>(score, 1.0f);

	potentialScore += score;
}
