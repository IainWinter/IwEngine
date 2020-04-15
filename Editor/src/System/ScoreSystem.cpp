#include "Systems/ScoreSystem.h"
#include "Events/ActionEvents.h"
#include "Components/LevelDoor.h"
#include "Components/Player.h"
#include "Components/Enemy.h"
#include "iw/engine/Time.h"
#include "Components/EnemyDeathCircle.h"

ScoreSystem::ScoreSystem(
	iw::Entity& player,
	iw::Camera* camera,
	iw::Camera* uiCam)
	: iw::System<iw::Transform, Score>("Score")
	, player(player)
	, totalScore(0.0f)
	, potentiaScore(0.0f)
	, camera(camera)
	, uiCam(uiCam)
{}

int ScoreSystem::Initialize() {
	// Fonts

	font = Asset->Load<iw::Font>("fonts/arial.fnt");
	//font->Initialize(Renderer->Device);

	iw::ref<iw::Shader> fontShader = Asset->Load<iw::Shader>("shaders/font.shader");
	Renderer->InitShader(fontShader, iw::CAMERA);

	textMatBad = REF<iw::Material>(fontShader);
	textMat    = REF<iw::Material>(fontShader);

	textMatBad->Set("color", iw::vector3(1, 0.1f, 0.1f));
	textMatBad->SetTexture("fontMap", font->GetTexture(0));
	textMatBad->SetTransparency(iw::Transparency::ADD);

	textMat   ->Set("color", iw::vector3(1));
	textMat   ->SetTexture("fontMap", font->GetTexture(0));
	textMat   ->SetTransparency(iw::Transparency::ADD);

	totalScoreMesh = font->GenerateMesh(std::to_string(totalScore), .02f, 1);
	totalScoreMesh.SetMaterial(textMat);
	totalScoreMesh.Data()->Initialize(Renderer->Device);

	potentialScoreMesh = font->GenerateMesh(std::to_string(potentiaScore), .02f, 1);
	potentialScoreMesh.SetMaterial(textMat);
	potentialScoreMesh.Data()->Initialize(Renderer->Device);

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
				itr->second->SetMaterial(textMatBad);
			}

			else if (score->Score >= 0) {
				itr->second->SetMaterial(textMat);
			}

			itr->second->Data()->Update(Renderer->Device);
		}

		iw::Transform trans = *transform;
		trans.Position.y +=  0.1f * (score->Timer / score->Lifetime);
		trans.Position.z += -2    * (score->Timer / score->Lifetime);

		trans.Rotation = iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2);

		trans.Scale = 1.0f;

		if (score->Timer >= score->Lifetime) {
			QueueDestroyEntity(entity.Index);
		}

		score->Timer += iw::Time::DeltaTime();

		Renderer->DrawMesh(trans, itr->second);
	}

	Renderer->EndScene();

	font->UpdateMesh(totalScoreMesh, std::to_string(totalScore), .01f, 1);
	totalScoreMesh.Data()->Update(Renderer->Device);

	font->UpdateMesh(potentialScoreMesh, std::to_string(potentiaScore), .01f, 1);
	potentialScoreMesh.Data()->Update(Renderer->Device);

	Renderer->BeginScene(uiCam);

	iw::Transform t;
	iw::Transform tp;
	t.Position = iw::vector3(-6.75, 5.5, 0);
	tp.Position = iw::vector3(-6.75, 4.75, 0);

	if (totalScore < 0) {
		totalScoreMesh.SetMaterial(textMatBad);
	}

	else if (totalScore >= 0) {
		totalScoreMesh.SetMaterial(textMat);
	}

	if (potentiaScore < 0) {
		potentialScoreMesh.SetMaterial(textMatBad);
	}

	else if (potentiaScore >= 0) {
		potentialScoreMesh.SetMaterial(textMat);
	}

	Renderer->DrawMesh(t,  totalScoreMesh);
	Renderer->DrawMesh(tp, potentialScoreMesh);

	Renderer->EndScene();
}

bool ScoreSystem::On(
	iw::CollisionEvent& event)
{
	iw::Entity a = Space->FindEntity(event.ObjA);
	if (a == iw::EntityHandle::Empty) {
		a = Space->FindEntity<iw::Rigidbody>(event.ObjA);
	}

	iw::Entity b = Space->FindEntity(event.ObjB);
	if (b == iw::EntityHandle::Empty) {
		b = Space->FindEntity<iw::Rigidbody>(event.ObjB);
	}

	iw::Entity bullet;
	iw::Entity other;
	if (   a != iw::EntityHandle::Empty 
		&& a.HasComponent<Bullet>()) 
	{
		bullet = a;
		other  = b;
	}

	else if (b != iw::EntityHandle::Empty
		  && b.HasComponent<Bullet>())
	{
		bullet = b;
		other  = a;
	}

	if (   other  == iw::EntityHandle::Empty
		|| bullet == iw::EntityHandle::Empty)
	{
		return false;
	}

 	if (other.HasComponent<EnemyDeathCircle>()) {
		iw::vector3 pos = bullet.FindComponent<iw::Rigidbody>()      ->Trans().Position;
		iw::vector3 des = other .FindComponent<iw::CollisionObject>()->Trans().Position;

		SpawnScore((pos-des).length() * 10, pos);
	}

	if (other.HasComponent<Player>()) {
		iw::vector3 pos = bullet.FindComponent<iw::Rigidbody>()->Trans().Position;

		SpawnScore(-potentiaScore * 0.5, pos);
	}

	return false;
}

bool ScoreSystem::On(
	iw::ActionEvent& e)
{
	if (e.Action == iw::val(Actions::GOTO_NEXT_LEVEL)) {
		totalScore += potentiaScore;
		potentiaScore = 0;
	}

	else if (e.Action == iw::val(Actions::RESET_LEVEL)) {
		potentiaScore = 0;
	}

	else if (e.Action == iw::val(Actions::SPAWN_ENEMY_DEATH)) {
		iw::vector3 pos = e.as<SpawnEnemyDeath>().Position;
		float       score = 1.0f - player.FindComponent<Player>()->Timer / player.FindComponent<Player>()->DashTime;

		SpawnScore(score * 1000, pos);
	}

	return false;
}

void ScoreSystem::SpawnScore(
	int score, 
	iw::vector3 position)
{
	iw::Entity entity = Space->CreateEntity<iw::Transform, Score>();

	entity.SetComponent<iw::Transform>(position);
	entity.SetComponent<Score>(score, 1.0f);

	potentiaScore += score;
}
