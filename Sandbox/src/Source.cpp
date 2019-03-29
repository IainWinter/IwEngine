#include "iw/engine/EntryPoint.h"
#include "iw/engine/ImGui/ImGuiLayer.h"
#include "iw/engine/Entity/EntityLayer.h"
#include "iw/entity/Space.h"

#include "iw/util/set/sparse_set.h"

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

		struct Comparator {
			bool operator()(
				const int& a, 
				const int& b) 
			{
				return a > b;
			}
		};

		iwu::sparse_set<unsigned int> set;
		
		set.emplace(3);
		set.emplace(7);
		set.emplace(4);
		set.emplace(1);
		set.emplace(9);
		set.sort(0, set.size() - 1, Comparator());

		IwEntity5::Space space;
		
		space.CreateComponent<Player>(4, 4.0f);

		space.CreateComponent<AI>(3, 3);

		space.CreateComponent<Collider>(3, 3);
		space.CreateComponent<Collider>(0, 0);
		space.CreateComponent<Collider>(2, 2);
		space.CreateComponent<Collider>(4, 4);
		space.CreateComponent<Collider>(1, 1);

		space.CreateComponent<Mesh>(1, 1);
		space.CreateComponent<Mesh>(2, 2);
		space.CreateComponent<Mesh>(4, 4);
		space.CreateComponent<Mesh>(3, 3);
		space.CreateComponent<Mesh>(0, 0);

		space.CreateComponent<Velocity>(4, 4.0f, 4.0f, 4.0f);
		space.CreateComponent<Velocity>(3, 3.0f, 3.0f, 3.0f);

		space.CreateComponent<Transform>(2, 2.0f, 2.0f, 2.0f);
		space.CreateComponent<Transform>(0, 0.0f, 0.0f, 0.0f);
		space.CreateComponent<Transform>(4, 4.0f, 4.0f, 4.0f);
		space.CreateComponent<Transform>(1, 1.0f, 1.0f, 1.0f);
		space.CreateComponent<Transform>(3, 3.0f, 3.0f, 3.0f);

		space.DestroyComponent<Mesh>(2);

		PushLayer(new IwEngine::EntityLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
