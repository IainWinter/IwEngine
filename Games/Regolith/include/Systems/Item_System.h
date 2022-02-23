#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "iw/physics/Collision/SphereCollider.h"
#include "Components/Item.h"
#include "Components/KeepInWorld.h"

#include "Events.h"
#include "Assets.h"
#include "ECS.h"

struct ItemSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	entity m_player;

	std::unordered_map<std::string, std::pair<int, float>> m_sequential; // name, count, last totaltime

	ItemSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Health")
		, sand(sand)
	{}

	void Update() override;
	void FixedUpdate() override;

	bool On(iw::ActionEvent& e) override;

private:
	entity MakeItem(const SpawnItem_Config& config, iw::ref<iw::Texture>& spritePath);

	entity MakeHealth     (const SpawnItem_Config& config);
	entity MakeLaserCharge(const SpawnItem_Config& config);
	entity MakePlayerCore (const SpawnItem_Config& config);

	// guns
	// regular - fast, slow firerate
	// minigun - slow, quick firerate
	// super laser - instance, charge up for seconds - cuts colliders & tiles in 

	entity MakeWeaponMinigun(const SpawnItem_Config& config);
	entity MakeWeaponWattz  (const SpawnItem_Config& config);
	entity MakeWeaponBoltz  (const SpawnItem_Config& config);
};
