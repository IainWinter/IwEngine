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
	, m_usingItem(false)
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
	m_slowmo.Material()->Set("reflectance", 1.0f);

	return 0;
}

void ConsumableSystem::Update(
	iw::EntityComponentArray& view)
{
	// this could be made much better

	iw::vector3 target = m_target.Find<iw::Transform>()->Position
		               + iw::vector3(-1.0f, 1.0f, 0.75f);
	
	float offset = 0.0f;
	float totalOffset = 0.0f;
	
	int index = 0;
	int count = 0;

	for (auto entity : view) {
		totalOffset += iw::Time::DeltaTimeScaled();
		count++;
	}

	for (auto entity : view) {
		auto [transform, consumable] = entity.Components.Tie<Components>();

		float time = offset + iw::TotalTime();

		iw::vector3 adj = target;
		float rot = (count - index + 1.0f) / count + 1.0f;

		if (Space->HasComponent<Slowmo>(entity.Handle)) {
			Slowmo* item = Space->FindComponent<Slowmo>(entity.Handle);

			if (item->Timer < 0) {
				iw::SetTimeScale(1.0f);
				QueueDestroyEntity(entity.Index);
				m_usingItem = false;
			}

			if (item->Timer > 0) {
				item->Timer -= iw::Time::DeltaTime();

				adj.y += iw::randf() * sin(time);
				adj.z += iw::randf() * sin(time);

				rot += 5;

				if (item->Timer + item->Time * 0.8f > item->Time) {
					transform->Scale = iw::lerp(transform->Scale, iw::vector3(iw::randf() + 0.25f), iw::Time::DeltaTime() * 8);
				}

				else {
					transform->Scale = iw::lerp(transform->Scale, iw::vector3(0), iw::Time::DeltaTime() * 5);
				}

				if (m_used) {
					iw::Time::SetTimeScale(0.3f);
				}

				else {
					iw::SetTimeScale(1.0f);
				}
			}

			else if (!m_usingItem && iw::Keyboard::KeyDown(iw::C)) {
				if (consumable->IsActive) {
					item->Timer = item->Time;
					m_used = true;
					m_usingItem = true;
				}
			}
		}
	
		transform->Position = iw::lerp(
			transform->Position, 
			adj + iw::vector3(0, 0.2f * sin(time), 0),
			((totalOffset - offset) * (totalOffset - offset) + iw::Time::DeltaTime()) * 5
		);

		transform->Rotation *= iw::quaternion::from_euler_angles(rot * iw::Time::DeltaTime());

		if (   m_usingItem
			&& index == 0)
		{
			target.x -= 0.25f;
		}

		target.x -= 0.5f;
		offset += iw::Time::DeltaTimeScaled();

		index++;
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
			if (m_used && !m_usingItem && m_activeConsumable > -1) {
				iw::Transform* consumable = SpawnConsumable(m_prefabs.at(m_activeConsumable));
				m_used = false;
			}

			break;
		}
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			if (m_used) {
				m_activeConsumable = -1;
			}

			// should tp items back to start

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
			consumable.Add<Slowmo>(3.0f);
		}
	}

	iw::Transform* t = consumable.Set<iw::Transform>(iw::vector3(m_target.Find<iw::Transform>()->Position), 0.25f);
					   consumable.Set<iw::Mesh>(m_slowmo.MakeInstance());
	                   consumable.Set<Consumable>(prefab);

	return t;
}
