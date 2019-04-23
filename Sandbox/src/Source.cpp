#include "iw/engine/EntryPoint.h"
#include "iw/engine/Entity/EntityLayer.h"

#include "iw/physics/Collision/Algorithm/GJK.h"

#include "iw/physics/Collision/AABB.h"

struct Transform { float x, y, z; };
struct Velocity  { float vx, vy, vz; };
struct Collider  { float count; };

class Game 
	: public IwEngine::Application
{
public:
	void Run() override {
		IwPhysics::AABB a = IwPhysics::AABB(iwm::vector3(0, 0, 0), 1);
		IwPhysics::AABB b = IwPhysics::AABB(iwm::vector3(1, 0, 0), 20);

		LOG_INFO << a.Intersects(b);
		LOG_INFO << b.Fits(a);

		Application::Run();
	}

	Game() {
		PushLayer(new IwEngine::EntityLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
