template<>
inline const Class* GetClass(ClassTag<event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("event", sizeof(event), 4);

		size_t offset = 0;

		c->fields[0] = { "Group", GetType(TypeTag<short>()), offset, false };
		offset += sizeof(short);

		c->fields[1] = { "Type", GetType(TypeTag<short>()), offset, false };
		offset += sizeof(short);

		c->fields[2] = { "Handled", GetType(TypeTag<bool>()), offset, false };
		offset += sizeof(bool);

		c->fields[3] = { "Size", GetType(TypeTag<int>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<ActionEvent>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("ActionEvent", sizeof(ActionEvent), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::event", GetClass(ClassTag<iw::event>()), offset, true };
		offset += sizeof(iw::event);

		c->fields[1] = { "Action", GetType(TypeTag<int>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<ToggleEvent>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("ToggleEvent", sizeof(ToggleEvent), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::Engine::ActionEvent", GetClass(ClassTag<iw::Engine::ActionEvent>()), offset, true };
		offset += sizeof(iw::Engine::ActionEvent);

		c->fields[1] = { "Active", GetType(TypeTag<bool>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SingleEvent>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SingleEvent", sizeof(SingleEvent), 1);

		size_t offset = 0;

		c->fields[0] = { "iw::Engine::ActionEvent", GetClass(ClassTag<iw::Engine::ActionEvent>()), offset, true };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<Transform>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("Transform", sizeof(Transform), 3);

		size_t offset = 0;

		c->fields[0] = { "Position", GetType(TypeTag<vec<3, float, defaultp>>()), offset, false };
		offset += sizeof(vec<3, float, defaultp>);

		c->fields[1] = { "Scale", GetType(TypeTag<vec<3, float, defaultp>>()), offset, false };
		offset += sizeof(vec<3, float, defaultp>);

		c->fields[2] = { "Rotation", GetType(TypeTag<qua<float, defaultp>>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SpawnExplosion_Config>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SpawnExplosion_Config", sizeof(SpawnExplosion_Config), 4);

		size_t offset = 0;

		c->fields[0] = { "SpawnLocationX", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[1] = { "SpawnLocationY", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[2] = { "ExplosionRadius", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[3] = { "ExplosionPower", GetType(TypeTag<float>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SpawnExplosion_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SpawnExplosion_Event", sizeof(SpawnExplosion_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Config", GetType(TypeTag<SpawnExplosion_Config>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SpawnEnemy_Config>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SpawnEnemy_Config", sizeof(SpawnEnemy_Config), 6);

		size_t offset = 0;

		c->fields[0] = { "TargetEntity", GetType(TypeTag<iw::Entity>()), offset, false };
		offset += sizeof(iw::Entity);

		c->fields[1] = { "SpawnLocationX", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[2] = { "SpawnLocationY", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[3] = { "TargetLocationX", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[4] = { "TargetLocationY", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[5] = { "EnemyType", GetType(TypeTag<EnemyType>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SpawnEnemy_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SpawnEnemy_Event", sizeof(SpawnEnemy_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Config", GetType(TypeTag<SpawnEnemy_Config>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SpawnAsteroid_Config>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SpawnAsteroid_Config", sizeof(SpawnAsteroid_Config), 6);

		size_t offset = 0;

		c->fields[0] = { "SpawnLocationX", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[1] = { "SpawnLocationY", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[2] = { "VelocityX", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[3] = { "VelocityY", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[4] = { "AngularVel", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[5] = { "Size", GetType(TypeTag<int>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SpawnAsteroid_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SpawnAsteroid_Event", sizeof(SpawnAsteroid_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Config", GetType(TypeTag<SpawnAsteroid_Config>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SpawnProjectile_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SpawnProjectile_Event", sizeof(SpawnProjectile_Event), 3);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Shot", GetType(TypeTag<ShotInfo>()), offset, false };
		offset += sizeof(ShotInfo);

		c->fields[2] = { "Depth", GetType(TypeTag<int>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SpawnItem_Config>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SpawnItem_Config", sizeof(SpawnItem_Config), 9);

		size_t offset = 0;

		c->fields[0] = { "X", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[1] = { "Y", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[2] = { "Amount", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[3] = { "ActivateDelay", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[4] = { "Speed", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[5] = { "AngularSpeed", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[6] = { "DieWithTime", GetType(TypeTag<bool>()), offset, false };
		offset += sizeof(bool);

		c->fields[7] = { "Item", GetType(TypeTag<ItemType>()), offset, false };
		offset += sizeof(ItemType);

		c->fields[8] = { "OnPickup", GetType(TypeTag<std::function<void ()>>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<SpawnItem_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("SpawnItem_Event", sizeof(SpawnItem_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Config", GetType(TypeTag<SpawnItem_Config>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<ChangeLaserFluid_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("ChangeLaserFluid_Event", sizeof(ChangeLaserFluid_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Amount", GetType(TypeTag<int>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<HealPlayer_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("HealPlayer_Event", sizeof(HealPlayer_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "IsCore", GetType(TypeTag<bool>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<ChangePlayerWeapon_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("ChangePlayerWeapon_Event", sizeof(ChangePlayerWeapon_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Weapon", GetType(TypeTag<WeaponType>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<ProjHitTile_Config>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("ProjHitTile_Config", sizeof(ProjHitTile_Config), 5);

		size_t offset = 0;

		c->fields[0] = { "X", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[1] = { "Y", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[2] = { "Info", GetType(TypeTag<iw::TileInfo>()), offset, false };
		offset += sizeof(iw::TileInfo);

		c->fields[3] = { "Hit", GetType(TypeTag<iw::Entity>()), offset, false };
		offset += sizeof(iw::Entity);

		c->fields[4] = { "Projectile", GetType(TypeTag<iw::Entity>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<ProjHitTile_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("ProjHitTile_Event", sizeof(ProjHitTile_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Config", GetType(TypeTag<ProjHitTile_Config>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<RemoveCellFromTile_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("RemoveCellFromTile_Event", sizeof(RemoveCellFromTile_Event), 3);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Index", GetType(TypeTag<size_t>()), offset, false };
		offset += sizeof(size_t);

		c->fields[2] = { "Entity", GetType(TypeTag<iw::Entity>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<CreatedPlayer_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("CreatedPlayer_Event", sizeof(CreatedPlayer_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "PlayerEntity", GetType(TypeTag<iw::Entity>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<CreatedCoreTile_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("CreatedCoreTile_Event", sizeof(CreatedCoreTile_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "TileEntity", GetType(TypeTag<iw::Entity>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<CoreExploded_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("CoreExploded_Event", sizeof(CoreExploded_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Entity", GetType(TypeTag<iw::Entity>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<StateChange_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("StateChange_Event", sizeof(StateChange_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "State", GetType(TypeTag<StateName>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<OpenMenu_Event>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("OpenMenu_Event", sizeof(OpenMenu_Event), 2);

		size_t offset = 0;

		c->fields[0] = { "iw::SingleEvent", GetClass(ClassTag<iw::SingleEvent>()), offset, true };
		offset += sizeof(iw::SingleEvent);

		c->fields[1] = { "Menu", GetType(TypeTag<MenuName>()), offset, false };
	}

	return c;
}

