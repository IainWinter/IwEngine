#include "Systems/Weapon_system.h"

void WeaponSystem::Update()
{
	Space->Query<Armorments>().Each([](
		iw::EntityHandle entity,
		Armorments* armorments) 
		{
			armorments->Timer.Tick();
		}
	);
}

bool WeaponSystem::On(iw::ActionEvent& e)
{
	if (   e.Action != CHANGE_WEAPON
		&& e.Action != FIRE_WEAPON)
	{
		return false;
	}

	switch (e.Action)
	{
		case CHANGE_WEAPON:
		{
			ChangeWeapon_Event& event = e.as<ChangeWeapon_Event>();
			Armorments* guns = event.Entity.Find<Armorments>();

			if (WeaponInfo* info = guns->GetWeapon(event.Weapon))
			{
				guns->CurrentWeapon = event.Weapon;
				guns->Ammo = event.WeaponAmmo;
				guns->Timer.SetTime("fire", info->FireTime, info->FireTimeMargin);
			}

			break;
		}

		case FIRE_WEAPON:
		{
			FireWeapon_Event& event = e.as<FireWeapon_Event>();
			Armorments* guns = event.Entity.Find<Armorments>();

			if (!guns->Timer.Can("fire")) break;

			if      (guns->Ammo  > 0) guns->Ammo -= 1;
			else if (guns->Ammo == 0) break;

			WeaponInfo* info = guns->GetWeapon(guns->CurrentWeapon);
			ShotInfo shot = info->GetShot(event.Entity, *info, event.TargetX, event.TargetY);
		
			Bus->push<SpawnProjectile_Event>(shot);

			break;
		}
	}

	return false;
}
