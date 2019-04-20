#include "iw/engine/EntryPoint.h"
//#include "iw/engine/Entity/EntityLayer.h"

#include "iw/entity/Space.h"
#include "iw/log/logger.h"

#include "iw/engine/Time.h"

struct Transform { float x, y, z; };
struct Velocity  { float vx, vy, vz; };
struct Collider  { float count; };

class Game 
	: public IwEngine::Application
{
public:
	Game() {
		//PushLayer(new IwEngine::EntityLayer());
	}

	void Run() override {
		IwEntity::Space space;

		IwEngine::Time::Update();

		for (int i = 0; i < 1000000; i++) {
			IwEntity::Entity e = space.CreateEntity();

			//if (rand() > RAND_MAX / 4.0f) {
				//space.CreateComponent<Transform>(e);
			//}

			//else if (rand() > RAND_MAX / 4.0f) {
			//	space.CreateComponent<Transform>(e);
			//	space.CreateComponent<Velocity>(e, (float)i, (float)i, (float)i);
			//}

			//else if (rand() > RAND_MAX / 4.0f) {
			//	space.CreateComponent<Transform>(e);
			//	space.CreateComponent<Collider>(e);
			//}

			//else if (rand() > RAND_MAX / 4.0f) {
			//	space.CreateComponent<Transform>(e);
			//	space.CreateComponent<Velocity>(e, (float)i, (float)i, (float)i);
			//	space.CreateComponent<Collider>(e);
			//}

		}

		LOG_INFO << IwEngine::Time::DeltaTime();

		space.Sort();

		auto view = space.ViewComponents<Transform, Velocity>();

		auto begin = view.begin();
		auto end   = view.end();

		for (auto entity : view) {
			Transform& transform = entity.GetComponent<Transform>();
			Velocity& velocity = entity.GetComponent<Velocity>();

			transform.x += velocity.vx;
			transform.y += velocity.vy;
			transform.z += velocity.vz;

			LOG_INFO << transform.x << ", " << transform.y << ", " << transform.z;
		}

		Application::Run();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
