#include "Systems/ConsumableSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Slowmo.h"

#include "iw/engine/Time.h"
#include "iw/engine/Components/UiElement_temp.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "iw/input/Devices/Keyboard.h"

ConsumableSystem::ConsumableSystem(
	iw::Entity& target)
	: iw::System<iw::Transform, Consumable>("Consumables")
	, m_target(target)
	, m_activeConsumable(-1)
	, m_used(false)
{}

int ConsumableSystem::Initialize() {
	m_prefabs.push_back(Consumable{ SLOWMO, true });

	iw::MeshDescription description;
	description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::NORMAL,   iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));

	m_slowmo = iw::MakeIcosphere(description, 0)->MakeInstance();
	m_slowmo.SetMaterial(Asset->Load<iw::Material>("materials/Default")->MakeInstance());

	m_slowmo.Material()->Set("baseColor", iw::Color::From255(0, 0, 255));

	return 0;
}

void ConsumableSystem::Update(
	iw::EntityComponentArray& view)
{
	iw::vector3 target = m_target.Find<iw::Transform>()->Position;
	target += iw::vector3(-0.75f, 1.0f, 0.75f);

	for (auto entity : view) {
		auto [transform, consumable] = entity.Components.Tie<Components>();

		transform->Position = iw::lerp(transform->Position, target, iw::Time::DeltaTimeScaled() * 5);

		if (Space->HasComponent<Slowmo>(entity.Handle)) {
			Slowmo* item = Space->FindComponent<Slowmo>(entity.Handle);

			if (item->Timer < 0) {
				iw::SetTimeScale(1.0f);
				QueueDestroyEntity(entity.Index);
			}

			if (item->Timer > 0) {
				item->Timer -= iw::Time::DeltaTime();
				if (m_used) {
					iw::Time::SetTimeScale(0.3f);
				}

				else {
					iw::SetTimeScale(1.0f);
				}
			}

			else if (iw::Keyboard::KeyDown(iw::C)) {
				if (consumable->IsActive) {
					item->Timer = item->Time;
					m_used = true;
				}
			}
		}
	}
}

bool ConsumableSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::SPAWN_CONSUMABLE): {
			SpawnConsumableEvent& event = e.as<SpawnConsumableEvent>();
			iw::Transform* consumable = SpawnConsumable(m_prefabs.at(event.Index));
			m_activeConsumable = event.Index;
			m_used = false;
			break;
		}
		case iw::val(Actions::RESET_LEVEL): {
			if (m_activeConsumable > -1) {
				iw::Transform* consumable = SpawnConsumable(m_prefabs.at(m_activeConsumable));
				m_used = false;
			}

			break;
		}
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			if (m_used) {
				m_activeConsumable = -1;
			}

			break;
		}
	}

	return false;
}

iw::Transform* ConsumableSystem::SpawnConsumable(
	Consumable prefab)
{
	iw::Entity consumable = Space->CreateEntity<iw::Transform, iw::Mesh, Consumable>();

	switch (prefab.Type) {
		case SLOWMO: {
			consumable.AddComponent<Slowmo>(3.0f);
		}
	}

	iw::Transform* t = consumable.Set<iw::Transform>(iw::vector3(6.5f, -3, 0), 0.25f);
					   consumable.Set<iw::Mesh>(m_slowmo.MakeInstance());
	                   consumable.Set<Consumable>(prefab);

	return t;
}
