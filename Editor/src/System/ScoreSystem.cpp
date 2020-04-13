#include "Systems/ScoreSystem.h"
#include "Events/ActionEvents.h"
#include "Components/LevelDoor.h"
#include "Components/Player.h"
#include "Components/Enemy.h"
#include "iw/engine/Time.h"

ScoreSystem::ScoreSystem()
	: iw::System<iw::Transform, iw::Font, Score>("Score")
{}

int ScoreSystem::Initialize() {
	// Fonts

	iw::ref<iw::Font> font = Asset->Load<iw::Font>("fonts/arial.fnt");
	font->Initialize(Renderer->Device);

	iw::Mesh* textMesh = font->GenerateMesh("", .01f, 1);

	iw::ref<iw::Shader> fontShader = Asset->Load<iw::Shader>("shaders/font.shader");
	Renderer->InitShader(fontShader, iw::CAMERA);

	iw::ref<iw::Material> textMat = REF<iw::Material>(fontShader);

	textMat->Set("color", iw::vector3(1, .25f, 1));
	textMat->SetTexture("fontMap", font->GetTexture(0));

	textMesh->SetMaterial(textMat);
	textMesh->Initialize(Renderer->Device);

	return  0;
}

void ScoreSystem::FixedUpdate(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, text, score] = entity.Components.Tie<Components>();

	}
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

	if (   other != iw::EntityHandle::Empty
		&& (   other.HasComponent<Bullet>() 
			|| other.HasComponent<Enemy>()/*.Index() == bullet.FindComponent<Bullet>()->enemyIndex*/
			|| other.FindComponent<LevelDoor>()))
	{
		return false;
	}

	if (bullet != iw::EntityHandle::Empty) {
		bullet.FindComponent<iw::Transform>()->SetParent(nullptr);
		Space->DestroyEntity(bullet.Index());
	}

	return false;
}
