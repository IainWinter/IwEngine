#include "Systems/Weapon_system.h"

void WeaponSystem::Update()
{
	Space->Query<Armorments>().Each([&](
		iw::EntityHandle entity,
		Armorments* gnus)
		{
			auto& [type, ammo, timer] = gnus->WeaponStack.back();
		}
	);
}

bool WeaponSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case CHANGE_WEAPON:
		{
			ChangeWeapon_Event& event = e.as<ChangeWeapon_Event>();
			Armorments* guns = event.Entity.Find<Armorments>();

			if (WeaponInfo* gun = guns->GetWeapon(event.Weapon))
			{
				auto [w, a, timer] = guns->WeaponStack.emplace_back(event.Weapon, event.WeaponAmmo);
				timer.SetTime("fire", gun->FireTime, gun->FireTimeMargin);
			}

			break;
		}

		case FIRE_WEAPON:
		{
			FireWeapon_Event& event = e.as<FireWeapon_Event>();
			Armorments* guns = event.Entity.Find<Armorments>();
	
			auto& [currentGun, ammo, timer] = guns->WeaponStack.back(); // should always be populated

			if (   !guns
				|| !timer.Can("fire"))
			{
				break;
			}

			WeaponInfo* gun = guns->GetWeapon(currentGun);
			
			if (!gun) {
				break;
			}

			if (ammo > 0) {
				ammo -= 1;
				LOG_INFO << ammo;
			}

			else if (ammo == 0) {
				guns->WeaponStack.pop_back();
				break;
			}

			Bus->push<SpawnProjectile_Event>(
				gun->GetShot(event.Entity, *gun, event.TargetX, event.TargetY)
			);

			break;
		}
	}

	return false;
}
