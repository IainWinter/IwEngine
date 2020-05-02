#include "Systems/ConsumableSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Slowmo.h"

#include "iw/engine/Time.h"
#include "iw/engine/Components/UiElement_temp.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "iw/input/Devices/Keyboard.h"

ConsumableSystem::ConsumableSystem()
	: iw::System<iw::Transform, Consumable>("Consumables")
{
}

int ConsumableSystem::Initialize() {
	return 0;
}

void ConsumableSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, note] = entity.Components.Tie<Components>();

		if (Space->HasComponent<Slowmo>(entity.Handle)) {
			Slowmo* item = Space->FindComponent<Slowmo>(entity.Handle);

			if (item->Timer <= 0) {
				iw::SetTimeScale(1.0f);
			}

			if (item->Timer > 0) {
				item->Timer -= iw::Time::DeltaTime();
				iw::Time::SetTimeScale(0.1f);
			}

			else if (iw::Keyboard::KeyDown(iw::X)) {
				if (item->IsActive) {
					item->Timer = item->Time;
				}

				if (item->IsPickedup) {
					item->IsActive = true;
				}
			}
		}
	}
}

bool ConsumableSystem::On(
	iw::ActionEvent& e)
{
	if (e.Action == iw::val(Actions::SPAWN_CONSUMABLE)) {
		SpawnConsumableEvent& event = e.as<SpawnConsumableEvent>();
		iw::Transform* note = SpawnConsumable(m_prefabs.at(event.Index));
		note->SetParent(m_root);
	}

	return false;
}

iw::Transform* ConsumableSystem::SpawnConsumable(
	Consumable prefab)
{
	iw::Entity note = Space->CreateEntity<iw::Transform, Consumable>();

	iw::Transform* t = note.Set<iw::Transform>(iw::vector3(-5, 3, 0));
	                   note.Set<Consumable>(prefab);
	return t;
}
