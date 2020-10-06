#include "Systems/ConsumableSystem.h"
#include "Events/ActionEvents.h"

#include "iw/engine/Time.h"
#include "iw/engine/Components/UiElement_temp.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "iw/input/Devices/Keyboard.h"
#include "Components/Player.h"

ConsumableSystem::ConsumableSystem(
	iw::Entity& target)
	: iw::SystemBase("Consumable")
	, m_target(target)
	, m_isActive(false)
{}

int ConsumableSystem::Initialize() {
	m_prefabs.push_back(Consumable{ 0, SLOWMO,      iw::Color::From255(112, 195, 255), detail::action_Slowmo, detail::effect_Slowmo, 3.0f });
	m_prefabs.push_back(Consumable{ 1, CHARGE_KILL, iw::Color::From255(255, 245, 112), detail::action_Charge, detail::effect_Slowmo, 3.0f });
	m_prefabs.push_back(Consumable{ 2, LONG_DASH,   iw::Color::From255(190,   3, 252), detail::action_LgDash, detail::effect_Slowmo, 32.0f / 60.0f });

	auto material = Asset->Load<iw::Material>("materials/Default")->MakeInstance();
	material->Set("reflectance", 1.0f);
	material->Set("emissive",    2.0f);

	iw::MeshDescription description;
	description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::NORMAL,   iw::MakeLayout<float>(3));
	description.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));

	m_mesh = iw::MakeIcosphere(description, 0)->MakeInstance();
	m_mesh.SetMaterial(material);

	return 0;
}

void ConsumableSystem::Update()
{
	auto entities = Space->Query<iw::Transform, Consumable>();

	iw::vector3 target = m_target.Find<iw::Transform>()->Position
					   + iw::vector3(-1.0f, 1.0f, 0.75f);

	entities.Each([&](
		auto entity,
		iw::Transform* transform,
		Consumable*    consumable)
	{
		transform->Rotation *= iw::quaternion::from_euler_angles(iw::Time::DeltaTime());
		transform->Position  = iw::lerp(transform->Position, target, iw::Time::DeltaTime() * 10);
		
		target = transform->Position;
		target.x -= 1.0f;
	});

	if (m_isActive) {
		entities.First([&](
			auto entity,
			iw::Transform* transform,
			Consumable* consumable)
		{
			consumable->Timer += iw::Time::DeltaTime();

			if (consumable->Timer >= consumable->Time) {
				consumable->Action(consumable, m_target, Bus.get(), true);

				m_used.push_back(entity);
				m_isActive = false;

				Space->KillEntity(entity);
			}

			else {
				                      consumable->Action(consumable, m_target, Bus.get(), false);
				iw::Transform delta = consumable->Effect(consumable, m_target);

				transform->Apply(delta);
				transform->Scale = delta.Scale;
			}

			m_active = entity;
		});
	}
	
	// no items

	if (entities.begin() == entities.end()) {
		m_isActive = false;
	}
}


bool ConsumableSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::USE): {
			if (!m_isActive) {
				m_isActive = true;
			}

			break;
		}
		case iw::val(Actions::SPAWN_CONSUMABLE): {
			iw::EntityHandle handle = SpawnConsumable(m_prefabs.at(e.as<SpawnConsumableEvent>().Index));
			m_justGot.push_back(handle);

			break;
		}
		case iw::val(Actions::RESET_LEVEL): {
			if (m_isActive) {
				Consumable* consumable = Space->FindComponent<Consumable>(m_active);
				consumable->Action(consumable, m_target, Bus.get(), true);

				Space->SetComponent<Consumable>(m_active, m_prefabs.at(consumable->Index));
			}

			for (iw::EntityHandle handle : m_used) {
				iw::Entity e = Space->ReviveEntity(handle);

				e.Set<iw::Transform>(m_target.Find<iw::Transform>()->Position, 0.25f);
				e.Set<Consumable>(m_prefabs.at(e.Find<Consumable>()->Index));
			}

			for (iw::EntityHandle handle : m_justGot) {
				Space->DestroyEntity(handle);
			}

			m_used.clear();
			m_justGot.clear();
			m_isActive = false;

			break;
		}
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			if (m_isActive) {
				Consumable* consumable = Space->FindComponent<Consumable>(m_active);
				consumable->Action(consumable, m_target, Bus.get(), true);

				Space->DestroyEntity(m_active);
			}

			for (iw::EntityHandle handle : m_used) {
				Space->DestroyEntity(handle);
			}

			m_used.clear();
			m_justGot.clear();
			m_isActive = false;

			break;
		}
	}

	return false;
}

iw::EntityHandle ConsumableSystem::SpawnConsumable(
	Consumable prefab)
{
	iw::Entity consumable = Space->CreateEntity<iw::Transform, iw::Model, Consumable>();

	switch (prefab.Type) {
		case SLOWMO: {
			consumable.Add<Slowmo>();
			break;
		}
		case CHARGE_KILL: {
			consumable.Add<ChargeKill>();
			break;
		}
	}

	iw::Mesh mesh = m_mesh.MakeInstance();
	mesh.Material()->Set("baseColor", prefab.Color);

	iw::Model model;
	model.AddMesh(mesh);

	consumable.Set<iw::Transform>(iw::vector3(m_target.Find<iw::Transform>()->Position), 0.25f);
	consumable.Set<Consumable>(prefab);
	consumable.Set<iw::Model>(model);

	return consumable.Handle;
}

namespace detail {
	void action_Slowmo(
		Consumable* slowmo,
		iw::Entity& target,
		iw::eventbus* bus,
		bool finish)
	{
		if (finish) {
			iw::Time::SetTimeScale(1.0f);
		}

		else {
			iw::Time::SetTimeScale(0.3f);
		}
	}

	void action_Charge(
		Consumable* charge,
		iw::Entity& target,
		iw::eventbus* bus,
		bool finish)
	{
		if (finish) {
			bus->push<ChargeKillEvent>(0.0f);
		}

		else {
			bus->push<ChargeKillEvent>(charge->Timer);
		}
	}

	void action_LgDash(
		Consumable* longDash, 
		iw::Entity& target, 
		iw::eventbus* bus, 
		bool finish)
	{
		Player* player = target.Find<Player>();

		//if (player && player->Dash) {
		//	longDash->Timer = 20 - 32 / 60.0f;
		//}

		if (finish) {
			bus->push<LongDashEvent>(false);
		}

		else {
			bus->push<LongDashEvent>(true);
		}
	}

	iw::Transform effect_Slowmo(
		Consumable* slowmo,
		iw::Entity& target)
	{
		iw::Transform delta;

		delta.Position.x = iw::randf() * 0.1f;
		delta.Position.z = iw::randf() * 0.1f;

		delta.Scale = (slowmo->Time - slowmo->Timer) / slowmo->Time * 0.1f + .1f;

		return delta;
	}

	iw::Transform effect_Charge( // doesnt really work because other items follow the first one maybe make it so that doesnt happen
		Consumable* slowmo,
		iw::Entity& target)
	{
		iw::Transform delta;

		delta.Position = target.Find<iw::Rigidbody>()->Velocity().normalized();

		return delta;
	}
}
