#include "iw/engine/EntryPoint.h"
//#include "iw/engine/Entity/EntityLayer.h"

#include "iw/entity/Space.h"

#include "iw/log/logger.h"

struct Transform { float x, y, z; };
struct Velocity  { float vx, vy, vz; };
struct Collider  { float count; };

class Game : public IwEngine::Application {
public:
	Game() {
		IwEntity::Space space;

		IwEntity::Entity e = space.CreateEntity();
		space.CreateComponent<Transform>(e);
		space.CreateComponent<Velocity>(e);
		space.CreateComponent<Collider>(e);

		auto view = space.GetComponents<Transform, Velocity>();
		
		for (auto a : view) {

		}

		LOG_INFO << "b";

		//PushLayer(new IwEngine::EntityLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
