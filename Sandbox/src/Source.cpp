#include "iw/engine/EntryPoint.h"
#include "iw/engine/Entity/EntityLayer.h"

#include "iw/entity/Space.h"

#include "iw/engine/Time.h"

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
