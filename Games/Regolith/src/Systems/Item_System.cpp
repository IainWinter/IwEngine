#include "Systems/Item_System.h"

void ItemSystem::FixedUpdate()
{
    iw::Transform& playerTrans = m_player.get<iw::Transform>();
    
    /*Space->Query<iw::Rigidbody, Item>().Each([&](
        iw::EntityHandle entity,
        iw::Rigidbody* rigidbody,
        Item* item) 
    {
            if (item->ActivateTimer > 0.f) {
                item->ActivateTimer -= iw::FixedTime();
                return;
            }

            if (item->DieWithTime)
            {
                item->LifeTimer -= iw::FixedTime();
                if (item->LifeTimer <= 0)
                {
                    Space->QueueEntity(entity, iw::func_Destroy);
                }
             
                else if (item->LifeTimer < 1)
                {
                    rigidbody->Transform.Scale = iw::lerp(rigidbody->Transform.Scale, glm::vec3(0.f), 1 - item->LifeTimer);
                }
            }

            glm::vec3 playerPos = playerTrans->Position;
            glm::vec3 healthPos = rigidbody->Transform.Position;

            float distance = glm::distance(healthPos, playerPos);

            if (item->PickingUp)
            {
                glm::vec3 vel = glm::normalize(playerPos - healthPos) * (300.f + 100 * item->PickupTimer);
                rigidbody->Velocity = iw::lerp(rigidbody->Velocity, vel, iw::FixedTime() * (10 + item->PickupTimer));
                item->PickupTimer += iw::FixedTime();

                if (distance < 5)
                {
                    if (item->OnPickUp) {
                        item->OnPickUp();

                        auto& [count, lasttime] = m_sequential[item->PickupAudio];
                        if (iw::TotalTime() - lasttime < .4f) count += 1;
                        else                                  count  = 0;
                        lasttime = iw::TotalTime();

                        float sequential = iw::clamp(count / 10.f, 0.f, 1.f);

                        PlaySound_Event event(item->PickupAudio);
                        event.Parameters["Sequential"] = sequential;
                        Bus->push<PlaySound_Event>(event);
                    }

                    else
                    {
                        LOG_WARNING << "Picked up item with no action!";
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
                rigidbody->Velocity        = iw::lerp(rigidbody->Velocity,        glm::vec3(0.f), item->MoveTimer / item->MoveTime);
                rigidbody->AngularVelocity = iw::lerp(rigidbody->AngularVelocity, glm::vec3(0.f), item->MoveTimer / item->MoveTime);
                item->MoveTimer += iw::FixedTime();
            }
        });*/

    for (auto [entity, rigidbody, item] : entities().query<iw::Rigidbody, Item>().with_entity())
    {
        if (item.ActivateTimer > 0.f) {
            item.ActivateTimer -= iw::FixedTime();
            return;
        }
        
        if (item.DieWithTime)
        {
            item.LifeTimer -= iw::FixedTime();
            if (item.LifeTimer <= 0)
            {
                entity.destroy();
            }
         
            else if (item.LifeTimer < 1)
            {
                rigidbody.Transform.Scale = iw::lerp(rigidbody.Transform.Scale, glm::vec3(0.f), 1 - item.LifeTimer);
            }
        }
        
        glm::vec3 playerPos = playerTrans.Position;
        glm::vec3 healthPos = rigidbody.Transform.Position;
        
        float distance = glm::distance(healthPos, playerPos);
        
        if (item.PickingUp)
        {
            glm::vec3 vel = glm::normalize(playerPos - healthPos) * (300.f + 100 * item.PickupTimer);
            rigidbody.Velocity = iw::lerp(rigidbody.Velocity, vel, iw::FixedTime() * (10 + item.PickupTimer));
            item.PickupTimer += iw::FixedTime();
        
            if (distance < 5)
            {
                if (item.OnPickUp) {
                    item.OnPickUp();
        
                    auto& [count, lasttime] = m_sequential[item.PickupAudio];
                    if (iw::TotalTime() - lasttime < .4f) count += 1;
                    else                                  count  = 0;
                    lasttime = iw::TotalTime();
        
                    float sequential = iw::clamp(count / 10.f, 0.f, 1.f);
        
                    PlaySound_Event event(item.PickupAudio);
                    event.Parameters["Sequential"] = sequential;
                    Bus->push<PlaySound_Event>(event);
                }
        
                else
                {
                    LOG_WARNING << "Picked up item with no action!";
                }
        
                entity.destroy();
            }
        
            else if (distance < 12)
            {
                rigidbody.Transform.Scale = iw::lerp(rigidbody.Transform.Scale, glm::vec3(1/3.f), 1 - distance / 12);
            }
        }
        
        else if (!item.PickingUp && distance < item.PickUpRadius)
        {
            item.PickingUp = true;
        }
        
        else if (item.MoveTimer > item.MoveTime)
        {
            rigidbody.Velocity = glm::vec3(0.f);
        }
        
        else {
            rigidbody.Velocity        = iw::lerp(rigidbody.Velocity,        glm::vec3(0.f), item.MoveTimer / item.MoveTime);
            rigidbody.AngularVelocity = iw::lerp(rigidbody.AngularVelocity, glm::vec3(0.f), item.MoveTimer / item.MoveTime);
            item.MoveTimer += iw::FixedTime();
        }
    }
}

bool ItemSystem::On(iw::ActionEvent& e)
{
    switch (e.Action)
    {
        case SPAWN_ITEM:
        {
             SpawnItem_Event& event = e.as<SpawnItem_Event>();

            float angle = 0.f;
            float angleStep = iw::Pi2 / event.Config.Amount;
            float angleRand = angleStep / 2 * iw::randfs();

            for (int i = 0; i < event.Config.Amount; i++)
            {
                float dx = cos(angle + angleRand);
                float dy = sin(angle + angleRand);
        
                angle += angleStep;

                entity entity;

                switch (event.Config.Item)
                {
                    case ItemType::HEALTH:         entity = MakeHealth       (event.Config); break;
                    case ItemType::LASER_CHARGE:   entity = MakeLaserCharge  (event.Config); break;
                    case ItemType::PLAYER_CORE:    entity = MakePlayerCore   (event.Config); break;

                    case ItemType::WEAPON_BOLTZ:   entity = MakeWeaponBoltz  (event.Config); break;
                    case ItemType::WEAPON_WATTZ:   entity = MakeWeaponWattz  (event.Config); break;
                    case ItemType::WEAPON_MINIGUN: entity = MakeWeaponMinigun(event.Config); break;
                }

                glm::vec3& vel = entity.get<iw::Rigidbody>().Velocity; 
                vel.x = dx * event.Config.Speed;
                vel.y = dy * event.Config.Speed;

                glm::vec3& avel = entity.get<iw::Rigidbody>().AngularVelocity;
                avel.z = iw::randfs() * event.Config.AngularSpeed;
            }

            break;
        }
        case CREATED_PLAYER: {
            m_player = e.as<CreatedPlayer_Event>().PlayerEntity;
            break;
        }
    }

    return false;
}

entity ItemSystem::MakeItem(
    const SpawnItem_Config& config,
    iw::ref<iw::Texture>& sprite)
{
    entity entity = MakeTile(*sprite, component_list<Item, KeepInWorld, iw::Circle>());
    AddEntityToPhysics(entity, Physics);

    Item&          item      = entity.get<Item>();
    iw::Transform& transform = entity.get<iw::Transform>();
	iw::Rigidbody& rigidbody = entity.get<iw::Rigidbody>();
    iw::Circle&    collider  = entity.get<iw::Circle>();
	iw::Tile&      tile      = entity.get<iw::Tile>();

    collider.Radius = glm::compMax(sprite->Dimensions())/*2.5f*/; // radius from sprite size
	
    transform.Position.x = config.X + iw::randi(10);
    transform.Position.y = config.Y + iw::randi(10);
    
    rigidbody.SetTransform(&transform);
    rigidbody.IsTrigger = true; // so they dont knock m_player around, I liked that effect tho but it could move m_player out of bounds

    tile.m_zIndex = 1;

    item.Type          = config.Item;
    item.ActivateTimer = config.ActivateDelay;
    item.OnPickUp      = config.OnPickup;
    item.DieWithTime   = config.DieWithTime;

    return entity;
}

entity ItemSystem::MakeHealth(const SpawnItem_Config& config)
{
    entity entity = MakeItem(config, A_texture_item_health);
    
    Item& item = entity.get<Item>();
    item.PickUpRadius = 50;
    item.OnPickUp = [&]()
    {
        Bus->push<HealPlayer_Event>();
        Bus->push<HealPlayer_Event>();
        Bus->push<HealPlayer_Event>();
    };

    item.PickupAudio = "event:/pickups/health";

    return entity;
}

entity ItemSystem::MakeLaserCharge(const SpawnItem_Config& config)
{
    entity entity = MakeItem(config, A_texture_item_energy);
    
    Item& item = entity.get<Item>();
    item.PickUpRadius = 50;
    item.OnPickUp = [&]()
    {
        Bus->push<ChangeLaserFluid_Event>(iw::randi(5) + 2);
    };

    return entity;
}

entity ItemSystem::MakePlayerCore(const SpawnItem_Config& config)
{
    entity entity = MakeItem(config, A_texture_item_coreShard);
	iw::Rigidbody& rigidbody = entity.get<iw::Rigidbody>();
	Item&          item      = entity.get<Item>();
    
    item.MoveTime = 5.f;
    item.PickUpRadius = 30;

    return entity;
}

entity ItemSystem::MakeWeaponWattz(const SpawnItem_Config& config)
{
    entity entity = MakeItem(config, A_texture_item_wattz);

    Item& item = entity.get<Item>();
    item.PickUpRadius = 25;
    item.OnPickUp = [&]()
    {
        Bus->push<ChangePlayerWeapon_Event>(WeaponType::WATTZ_CANNON);
    };

    return entity;
}

entity ItemSystem::MakeWeaponBoltz(const SpawnItem_Config& config)
{
    entity entity = MakeItem(config, A_texture_item_boltz);

    Item& item = entity.get<Item>();
    item.PickUpRadius = 25;
    item.OnPickUp = [&]()
    {
        Bus->push<ChangePlayerWeapon_Event>(WeaponType::BOLTZ_CANNON);
    };

    return entity;
}

entity ItemSystem::MakeWeaponMinigun(const SpawnItem_Config& config)
{
    entity entity = MakeItem(config, A_texture_item_minigun);

    Item& item = entity.get<Item>();
    item.PickUpRadius = 25;
    item.OnPickUp = [&]()
    {
        Bus->push<ChangePlayerWeapon_Event>(WeaponType::MINIGUN_CANNON);
    };

    return entity;
}
