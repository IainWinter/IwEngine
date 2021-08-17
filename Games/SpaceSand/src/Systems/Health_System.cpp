#include "Systems/Health_System.h"

void HealthSystem::FixedUpdate()
{
    Space->Query<iw::Rigidbody, Health>().Each([&](
        iw::EntityHandle entity,
        iw::Rigidbody* rigidbody,
        Health* health) 
        {
            glm::vec3 playerPos = m_player.Find<iw::Transform>()->Position;
            glm::vec3 healthPos = rigidbody->Transform.Position;

            float distance = glm::distance(healthPos, playerPos);

            if (health->Pickedup)
            {
                // should calcuate an arc so they cannot orbit

                glm::vec3 vel = glm::normalize(playerPos - healthPos) * 200.f;
                rigidbody->Velocity = iw::lerp(rigidbody->Velocity, vel, iw::FixedTime() * 10);

                if (distance < 5)
                {
                    m_player.Find<iw::Tile>()->ReinstateRandomPixel();
                    Space->QueueEntity(entity, iw::func_Destroy);
                }

                else if (distance < 12)
                {
                    rigidbody->Transform.Scale = iw::lerp(rigidbody->Transform.Scale, glm::vec3(1/3.f), 1 - distance / 12);
                }
            }

            else if (!health->Pickedup && distance < 50)
            {
                health->Pickedup = true;
            }

            else if (health->MoveTimer > health->MoveTime)
            {
                rigidbody->Velocity = glm::vec3(0.f);
            }

            else {
                rigidbody->Velocity = iw::lerp(rigidbody->Velocity, glm::vec3(0.f), health->MoveTimer / health->MoveTime);
                health->MoveTimer += iw::FixedTime();
            }
        });
}

bool HealthSystem::On(iw::ActionEvent& e)
{
    switch (e.Action) {
        case SPAWN_HEALTH:
        {
            SpawnHealth_Event& event = e.as<SpawnHealth_Event>();

            float angle = 0.f;
            float angleStep = iw::Pi2 / event.Amount;
            float angleRand = angleStep / 2 * iw::randf();

            for (int i = 0; i < event.Amount; i++)
            {
                float dx = cos(angle + angleRand);
                float dy = sin(angle + angleRand);

                angle += angleStep;

                MakeHealth(event.X, event.Y, dx, dy);
            }

            break;
        }
    }

    return false;
}

iw::Entity HealthSystem::MakeHealth(
    float   x, float   y, 
    float ndx, float ndy)
{
    iw::Entity entity = sand->MakeTile<iw::Circle, Health>("textures/SpaceGame/health_big.png", true);
    
                               entity.Set<Health>();
    iw::Transform* transform = entity.Find<iw::Transform>();
	iw::Rigidbody* rigidbody = entity.Find<iw::Rigidbody>();
    iw::Circle*    collider  = entity.Find<iw::Circle>();
	iw::Tile*      tile      = entity.Find<iw::Tile>();

    collider->Radius = 2.5f;
	
    tile->m_zIndex = 1;
	
    transform->Position.x = x + iw::randi(10);
    transform->Position.y = y + iw::randi(10);
    
    rigidbody->Velocity.x = ndx * 75;
    rigidbody->Velocity.y = ndy * 75;
    rigidbody->SetTransform(transform);

    return iw::Entity();
}
