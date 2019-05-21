#include "iw/engine/EntryPoint.h"
#include "iw/engine/Entity/EntityLayer.h"

#include "iw/physics/2D/Collision/Quadtree.h"

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
	IwPhysics::Quadtree<int> tree;

	return new Game();
}
