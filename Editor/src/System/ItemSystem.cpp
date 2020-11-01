#include "Systems/ItemSystem.h"
#include "Events/ActionEvents.h"

#include "Components/Player.h"
#include "Components/Note.h"

#include "iw/engine/Time.h"
#include "iw/physics/Collision/SphereCollider.h";
#include "iw/physics/Collision/CollisionObject.h";
#include "iw/audio/AudioSpaceStudio.h"

ItemSystem::ItemSystem(
	GameSaveState* saveState)
	: iw::SystemBase("Item")
	, m_saveState(saveState)
{}

int ItemSystem::Initialize() {
	iw::MeshDescription   description = Asset->Load<iw::Model>("Sphere")->GetMesh(0).Data()->Description();
	iw::ref<iw::Material> material = Asset->Load<iw::Material>("materials/Default")->MakeInstance();

	iw::Mesh note = iw::MakePlane(description, 1, 1)->MakeInstance();

	//material->Set("baseColor", iw::Color::From255(209, 195, 167));
	//material->Set("emissive", 1.2f);
	//material->Set("reflectance", 1.0f);

	material->Set("reflectance", 0.0f);
	material->Set("emissive", 1.0f);
	material->Set("indirectDiffuse", 1);
	material->Set("indirectSpecular", 1);

	//material->SetCastShadows(false);
	note.SetMaterial(material);

	m_noteModel.AddMesh(note);

	return 0;
}

bool ItemSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();

			if (event.LevelName == "levels/forest/forest05.a.json") {
				Bus->push<SpawnItemEvent>(Item{ NOTE,       0 }, iw::vector3(3, 1, -2), event.Level);
				Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 0 }, iw::vector3(0, 1,  3), event.Level);
			}

			else if (event.LevelName == "levels/forest/forest07.a.json") {
				Bus->push<SpawnItemEvent>(Item{ NOTE,       1 }, iw::vector3(24, 1, 8), event.Level);
				Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 0 }, iw::vector3( 8, 1, 0), event.Level);
			}

			else if (event.LevelName == "levels/forest/forest12.a.json") {
				Bus->push<SpawnItemEvent>(Item{ NOTE,       2 }, iw::vector3(3, 1, -2), event.Level);
				Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 1 }, iw::vector3(0, 1,  3), event.Level);
			}

			else if (event.LevelName == "levels/canyon/cave04.json") {
				Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 2 }, iw::vector3(22, 1, 7.5f), event.Level, "Cave04ItemPickedUp");
			}

			else if (event.LevelName == "levels/canyon/cave06.json") {
				Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 2 }, iw::vector3(4, 1, -9.5f), event.Level, "Cave06ItemPickedUp");
			}

			else if (event.LevelName == "levels/canyon/canyon07.json") {
				Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 0 }, iw::vector3(0, 1, 9), event.Level, "Canyon07ItemPickedUp");
			}

			break;
		}
		case iw::val(Actions::SPAWN_ITEM): {
			SpawnItemEvent& event = e.as<SpawnItemEvent>();

			iw::Transform* note = SpawnItem(event.Item, event.Position, event.SaveState);
			if (note) {
				note->SetParent(event.Level);
			}

			break;
		}
		case iw::val(Actions::RESET_LEVEL): {
			Space->Query<Item>().Each([&](auto, Item* item) {
				m_saveState->SetState(item->SaveState, false);
			});

			break;
		}
	}

	return false;
}

bool ItemSystem::On(
	iw::CollisionEvent& e)
{
	iw::Entity itemEntity, otherEntity;
	if (GetEntitiesFromManifold<Item>(e.Manifold, itemEntity, otherEntity)) {
		return false;
	}

	if (otherEntity.Has<Player>()) {
		Item* item = itemEntity.Find<Item>();

		switch (item->Type) {
			case NOTE: {
				Bus->push<SpawnNoteEvent>(item->Id);
				break;
			}
			case CONSUMABLE: {
				m_saveState->SetState(item->SaveState, true);
				Bus->push<SpawnConsumableEvent>(item->Id);
				break;
			}
			default: {
				LOG_WARNING << "Tried to spawn item with an invalid type";
				break;
			}
		}

		itemEntity.Find<iw::CollisionObject>()->Trans().Position.y = -500; // so save can be reset this seems so stupid
	}

	return false;
}

iw::Transform* ItemSystem::SpawnItem(
	Item prefab,
	iw::vector3 position,
	std::string saveState)
{
	if (m_saveState->GetState(saveState)) {
		return nullptr;
	}

	iw::Entity entity = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::CollisionObject, Item>();

	Item*                i = entity.Set<Item>(prefab);
	                         entity.Set<iw::Model>(m_noteModel);
	iw::Transform*       t = entity.Set<iw::Transform>(position, iw::vector3(0.65f, 0.9f, 0.9f), iw::quaternion::from_euler_angles(0, iw::Pi * 2 * rand() / RAND_MAX, 0));
	iw::SphereCollider*  s = entity.Set<iw::SphereCollider>(iw::vector3::zero, 1);
	iw::CollisionObject* c = entity.Set<iw::CollisionObject>();

	i->SaveState = saveState;

	c->SetCol(s);
	c->SetTrans(t);
	c->SetIsTrigger(true);

	Physics->AddCollisionObject(c);

	return t;
}
