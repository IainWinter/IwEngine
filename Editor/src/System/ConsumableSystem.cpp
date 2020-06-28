#include "Systems/ConsumableSystem.h"
#include "Events/ActionEvents.h"

#include "iw/engine/Time.h"
#include "iw/engine/Components/UiElement_temp.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "iw/input/Devices/Keyboard.h"

ConsumableSystem::ConsumableSystem(
	iw::Entity& target)
	: iw::System<iw::Transform, Consumable>("Consumables")
	, m_target(target)
	, m_usingItem(false)
{}

int ConsumableSystem::Initialize() {
	m_prefabs.push_back(Consumable{ 0, SLOWMO,      3.0f });
	m_prefabs.push_back(Consumable{ 1, CHARGE_KILL, 3.0f });

	iw::MeshDescription description;
	description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::NORMAL,   iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));

	// slow mo

	iw::Mesh slowmo = iw::MakeIcosphere(description, 0)->MakeInstance();
	slowmo.SetMaterial(Asset->Load<iw::Material>("materials/Default")->MakeInstance());

	slowmo.Material()->Set("baseColor", iw::Color::From255(112, 195, 255));
	slowmo.Material()->Set("reflectance", 1.0f);
	slowmo.Material()->Set("emissive", 2.0f);

	m_slowmoModel.AddMesh(slowmo);

	// super charge

	iw::Mesh chargedKill = iw::MakeIcosphere(description, 0)->MakeInstance();
	chargedKill.SetMaterial(Asset->Load<iw::Material>("materials/Default")->MakeInstance());

	chargedKill.Material()->Set("baseColor", iw::Color::From255(255, 245, 112));
	chargedKill.Material()->Set("reflectance", 1.0f);
	chargedKill.Material()->Set("emissive", 2.0f);

	m_chargedKillModel.AddMesh(chargedKill);

	return 0;
}

void ConsumableSystem::Update(
	iw::EntityComponentArray& view)
{
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
		adj.y += 0.2f * sin(time);
		
		float rot = (count - index + 1.0f) / count + 1.0f;

		if (consumable->Timer < 0) {
			iw::SetTimeScale(1.0f);

			Bus->push<ChargeKillEvent>(0.0f);
			Space->QueueEntity(entity.Handle, iw::func_Destroy);

			m_used.push_back(entity.Handle);
			m_usingItem = false;
		}

		else if (consumable->Timer > 0) {
			consumable->Timer -= iw::Time::DeltaTime();

			rot += 5;

			if (consumable->Timer + consumable->Time * 0.8f > consumable->Time) {
				transform->Scale = iw::lerp(transform->Scale, iw::vector3(iw::randf() * 0.2f + 0.3f), iw::Time::DeltaTime() * 8);
			}
			
			else {
				transform->Scale = iw::lerp(transform->Scale, iw::vector3(0), iw::Time::DeltaTime() * 5);
			}

			switch (consumable->Type) {
				case SLOWMO: {
					adj.y += iw::randf() * sin(time);
					adj.z += iw::randf() * sin(time);

					iw::Time::SetTimeScale(0.3f);

					break;
				}
				case CHARGE_KILL: {
					if (m_target.Find<iw::Rigidbody>()->Velocity().length_squared() != 0) {
						adj = m_target.Find<iw::Transform>()->Position
							+ m_target.Find<iw::Rigidbody>()->Velocity().normalized() * 2.0f;
					}

					Bus->push<ChargeKillEvent>(consumable->Timer);
					break;
				}
			}
		}

		else if (!m_usingItem && iw::Keyboard::KeyDown(iw::C)) {
			consumable->Timer = consumable->Time;
			m_usingItem = true;
		}
	
		transform->Position = iw::lerp(
			transform->Position, 
			adj,
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

			break;
		}
		case iw::val(Actions::RESET_LEVEL): {
			for (iw::EntityHandle handle : m_used) {
				iw::Entity e = Space->ReviveEntity(handle);

				iw::Transform* t = e.Find<iw::Transform>();
				t->Scale = 0.25f;
				t->Position = m_target.Find<iw::Transform>()->Position;

				Consumable* c = e.Find<Consumable>();
				*c = m_prefabs.at(c->Index);
			}

			m_used.clear();

			break;
		}
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			for (iw::EntityHandle handle : m_used) {
				Space->DestroyEntity(handle);
			}

			m_used.clear();

			break;
		}
	}

	return false;
}

iw::Transform* ConsumableSystem::SpawnConsumable(
	Consumable prefab)
{
	iw::Entity consumable = Space->CreateEntity<iw::Transform, iw::Model, Consumable>();

	switch (prefab.Type) {
		case SLOWMO: {
			consumable.Add<Slowmo>();
			consumable.Set<iw::Model>(m_slowmoModel);
			break;
		}
		case CHARGE_KILL: {
			consumable.Add<ChargeKill>();
			consumable.Set<iw::Model>(m_chargedKillModel);
			break;
		}
	}

	iw::Transform* t = consumable.Set<iw::Transform>(iw::vector3(m_target.Find<iw::Transform>()->Position), 0.25f);
	                   consumable.Set<Consumable>(prefab);

	return t;
}
