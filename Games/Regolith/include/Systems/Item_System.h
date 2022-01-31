#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "iw/physics/Collision/SphereCollider.h"
#include "Components/Item.h"
#include "Components/KeepInWorld.h"

#include "Events.h"
#include "Assets.h"

struct ItemSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::Entity m_player;

	std::unordered_map<std::string, std::pair<int, float>> m_sequential; // name, count, last totaltime


	ItemSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Health")
		, sand(sand)
	{}

	void FixedUpdate() override;

	bool On(iw::ActionEvent& e) override;

private:
	iw::Entity MakeItem(const SpawnItem_Config& config, iw::ref<iw::Texture>& spritePath);

	iw::Entity MakeHealth     (const SpawnItem_Config& config);
	iw::Entity MakeLaserCharge(const SpawnItem_Config& config);
	iw::Entity MakePlayerCore (const SpawnItem_Config& config);

	// guns
	// regular - fast, slow firerate
	// minigun - slow, quick firerate
	// super laser - instance, charge up for seconds - cuts colliders & tiles in 

	iw::Entity MakeWeaponMinigun(const SpawnItem_Config& config);
	iw::Entity MakeWeaponWattz  (const SpawnItem_Config& config);
	iw::Entity MakeWeaponBoltz  (const SpawnItem_Config& config);
};
