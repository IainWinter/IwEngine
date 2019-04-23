#include "iw/engine/EntryPoint.h"
#include "iw/engine/Entity/EntityLayer.h"

#include "iw/physics/Collision/Algorithm/GJK.h"

struct Transform { float x, y, z; };
struct Velocity  { float vx, vy, vz; };
struct Collider  { float count; };

class Game 
	: public IwEngine::Application
{
public:
	Game() {
		PushLayer(new IwEngine::EntityLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
