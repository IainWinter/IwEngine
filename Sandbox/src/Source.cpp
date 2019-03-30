#include "iw/engine/EntryPoint.h"
#include "iw/engine/ImGui/ImGuiLayer.h"
#include "iw/engine/Entity/EntityLayer.h"
#include "iw/entity/Space.h"

struct AI        { int x; };
struct Player    { float speed; };
struct Collider  { int count; };
struct Mesh      { int count; };
struct Velocity  { float x, y, z; };
struct Transform { float x, y, z; };

class Game : public IwEngine::Application {
public:
	Game() {
		//Sort by count of components and then entity
		//
		// P 4
		// A 3
		// C 3 0 2 4 1
		// M 1 2 4 3 0
		// V 4 3
		// T 2 0 4 1 3
		//
		//Sorts to
		//
		// P 4
		// A 3
		// C 3 4 0 1 2
		// M 3 4 0 1 2
		// V 3 4
		// T 3 4 0 1 2
		//
		// Get components to iterate
		// 1. Get all sets that match
		// 2. Get size of shortest set
		// 3. If all sets have the same size
		//  3 t. Start at the first component
		//  3 f. Start at the first component with its entity matching the archetype
		// 4. End after the number of components visited equals the number of entites found matching the archetype
		//
		// Archetypes
		// P C M V T
		// A C M V T
		// C M T
		//
		// Systems
		// P T   -- Player
		// A V T -- Ai
		// V T   -- Physics dynamics
		// C T   -- Physics collisions
		// M T   -- Rendering
		//
		//IwEntity3::Entity entity = space.CreateEntity();

		PushLayer(new IwEngine::EntityLayer());
	}
 
	void Run() override {
		IwEntity5::Space space;

		IwEntity5::Entity player = space.CreateEntity();
		space.CreateComponent<Transform>(player, 4.0f, 4.0f, 4.0f);
		space.CreateComponent<Velocity>(player, 4.0f, 4.0f, 4.0f);
		space.CreateComponent<Collider>(player, 4);
		space.CreateComponent<Mesh>(player, 4);
		space.CreateComponent<Player>(player, 4.0f);

		for (int i = 0; i < 10; i++) {
			IwEntity5::Entity e = space.CreateEntity();
			space.CreateComponent<Transform>(e, 3.0f, 3.0f, 3.0f);

			if (rand() / (float)RAND_MAX > 0.6f) {
				space.CreateComponent<AI>(e, 3);
			}

			if (rand() / (float)RAND_MAX > 0.5f) {
				space.CreateComponent<Collider>(e, 3);
			}

			if (rand() / (float)RAND_MAX > 0.5f) {
				space.CreateComponent<Mesh>(e, 3);
			}			
		}

		space.Sort();
		space.Log();

		auto itr = space.GetComponents<Transform, Collider>();

		Application::Run();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}

//Space
// Transform
//  Entity: Value
