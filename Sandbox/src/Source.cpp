#include "iw/engine/EntryPoint.h"
//#include "iw/engine/Entity/EntityLayer.h"

#include "iw/entity/Space.h"
#include "iw/log/logger.h"

#include "iw/util/tuple/index.h"

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

		for (int i = 0; i < 10; i++) {
			IwEntity::Entity e = space.CreateEntity();

			space.CreateComponent<Transform>(e);

			if (rand() > RAND_MAX / 2.0f) {
				space.CreateComponent<Velocity>(e, (float)i, (float)i, (float)i);
			}

			if (rand() > RAND_MAX / 2.0f) {
				space.CreateComponent<Collider>(e);
			}

		}

		//space.Sort();
		//space.Log();

		//auto view = space.ViewComponents<Transform, Velocity>();

		//for (auto entity : view) {
		//	Transform& transform = entity.GetComponent<Transform>();
		//	Velocity& velocity   = entity.GetComponent<Velocity>();

		//	transform.x += velocity.vx;
		//	transform.y += velocity.vy;
		//	transform.z += velocity.vz;

		//	LOG_INFO << transform.x << ", " << transform.y << ", " << transform.z;
		//}

		Application::Run();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
