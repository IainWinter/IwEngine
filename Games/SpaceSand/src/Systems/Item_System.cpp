#include "Systems/Item_System.h"

void ItemSystem::FixedUpdate()
{
    Space->Query<iw::Rigidbody, Item>().Each([&](
        iw::EntityHandle entity,
        iw::Rigidbody* rigidbody,
        Item* item) 
        {
            if (item->ActivateTimer > 0.f) {
                item->ActivateTimer -= iw::FixedTime();
                return;
            }

            glm::vec3 playerPos = m_player.Find<iw::Transform>()->Position;
            glm::vec3 healthPos = rigidbody->Transform.Position;

            float distance = glm::distance(healthPos, playerPos);

            if (item->PickingUp)
            {
                // should calcuate an arc so they cannot orbit

                glm::vec3 vel = glm::normalize(playerPos - healthPos) * 250.f;
                rigidbody->Velocity = iw::lerp(rigidbody->Velocity, vel, iw::FixedTime() * 10);

                if (distance < 5)
                {
                    if (item->OnPickUp) {
                        item->OnPickUp();
                    }
                    else {
                        LOG_WARNING << "Pickedup item with no action!";
                    }

                    Space->QueueEntity(entity, iw::func_Destroy);
                }

                else if (distance < 12)
                {
                    rigidbody->Transform.Scale = iw::lerp(rigidbody->Transform.Scale, glm::vec3(1/3.f), 1 - distance / 12);
                }
            }

            else if (!item->PickingUp && distance < item->PickUpRadius)
            {
                item->PickingUp = true;
            }

            else if (item->MoveTimer > item->MoveTime)
            {
                rigidbody->Velocity = glm::vec3(0.f);
            }

            else {
                rigidbody->Velocity = iw::lerp(rigidbody->Velocity, glm::vec3(0.f), item->MoveTimer / item->MoveTime);
                item->MoveTimer += iw::FixedTime();
            }
        });
}

bool ItemSystem::On(iw::ActionEvent& e)
{
    if (e.Action != SPAWN_ITEM) return false;

    SpawnItem_Event& event = e.as<SpawnItem_Event>();

    float angle = 0.f;
    float angleStep = iw::Pi2 / event.Config.Amount;
    float angleRand = angleStep / 2 * iw::randfs();

    for (int i = 0; i < event.Config.Amount; i++)
    {
        float dx = cos(angle + angleRand);
        float dy = sin(angle + angleRand);

        angle += angleStep;

        iw::Entity entity;

        switch (event.Config.Item)
        {
            case ItemType::HEALTH:         entity = MakeHealth       (event.Config); break;
            case ItemType::LASER_CHARGE:   entity = MakeLaserCharge  (event.Config); break;
            case ItemType::WEAPON_MINIGUN: entity = MakeWeaponMinigun(event.Config); break;
            case ItemType::PLAYER_CORE:    entity = MakePlayerCore   (event.Config); break;
        }

        glm::vec3& vel = entity.Find<iw::Rigidbody>()->Velocity; 
        vel.x = dx * event.Config.Speed;
        vel.y = dy * event.Config.Speed;
    }

    return false;
}

iw::Entity ItemSystem::MakeItem(
    const SpawnItem_Config& config,
    iw::ref<iw::Texture>& sprite)
{
    iw::Entity entity = sand->MakeTile<iw::Circle, Item, KeepInWorld>(sprite, true);
    
    Item*          item      = entity.Set<Item>();
    iw::Transform* transform = entity.Find<iw::Transform>();
	iw::Rigidbody* rigidbody = entity.Find<iw::Rigidbody>();
    iw::Circle*    collider  = entity.Find<iw::Circle>();
	iw::Tile*      tile      = entity.Find<iw::Tile>();

    collider->Radius = glm::compMax(sprite->Dimensions())/*2.5f*/; // radius from sprite size
	
    transform->Position.x = config.X + iw::randi(10);
    transform->Position.y = config.Y + iw::randi(10);
    
    rigidbody->SetTransform(transform);
    rigidbody->IsTrigger = true; // so they dont knock m_player around, I liked that effect tho but it could move m_player out of bounds

    tile->m_zIndex = 1;

    item->ActivateTimer = config.ActivateDelay;
    item->OnPickUp = config.OnPickup;

    return entity;
}

iw::Entity ItemSystem::MakeHealth(const SpawnItem_Config& config)
{
    iw::Entity entity = MakeItem(config, A_texture_item_health);
    
    Item* item = entity.Find<Item>();
    item->PickUpRadius = 50;
    item->OnPickUp = [&]()
    {
        Bus->push<HealPlayer_Event>();
    };

    return entity;
}

iw::Entity ItemSystem::MakeLaserCharge(const SpawnItem_Config& config)
{
    iw::Entity entity = MakeItem(config, A_texture_item_energy);
    
    Item* item = entity.Find<Item>();
    item->PickUpRadius = 50;
    item->OnPickUp = [&]()
    {
        Bus->push<ChangeLaserFluid_Event>(iw::randi(5) + 2);
    };

    return entity;
}

iw::Entity ItemSystem::MakePlayerCore(const SpawnItem_Config& config)
{
    iw::Entity entity = MakeItem(config, A_texture_star);
	iw::Rigidbody* rigidbody = entity.Find<iw::Rigidbody>();
	Item*          item      = entity.Find<Item>();
    
    rigidbody->Velocity += glm::vec3(iw::randfs(), iw::randfs(), 0) * 100.f;

    item->MoveTime = 5.f;
    item->PickUpRadius = 30;

    return entity;
}

iw::Entity ItemSystem::MakeWeaponMinigun(const SpawnItem_Config& config)
{
    iw::Entity entity = MakeItem(config, A_texture_item_minigun);

    Item* item = entity.Find<Item>();
    item->PickUpRadius = 25;
    item->OnPickUp = [&]()
    {
        Bus->push<ChangePlayerWeapon_Event>(WeaponType::MINIGUN_CANNON);
    };

    return entity;
}
