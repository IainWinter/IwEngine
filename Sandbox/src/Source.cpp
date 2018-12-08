#include <iostream>

#include "IwEngine/IwEngine.h"
#include "IwEcs/ecs_manager.h"
#include "IwUtil/pack_input_iterator.h"

#include <vector>
#include <iostream>

class intfloat_system {
public:
	void update() {
		std::vector<int> ints = std::vector<int>();

		ints.push_back(1);
		ints.push_back(2);
		ints.push_back(3);

		std::vector<int>::iterator itr = ints.begin();
		std::vector<int>::iterator end = ints.end();

		for (; itr != end; itr++) {
			std::cout << *itr << std::endl;
		}
	}
};

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	struct test {
		template<typename _t>
		void operator()(_t& t, _t t1, int i) {
			t += t1 * i;
		}
	};

	void Run() override {
		using tup_t = std::tuple<int, double>;

		tup_t id = tup_t(1, 2);
		tup_t id_r = tup_t(2, 1);

		iwutil::foreach<test, tup_t, tup_t, 2>(id, id_r, 5);

		iwecs::ecs_manager m = iwecs::ecs_manager();
		
		for (int i = 0; i < 500; i++) {
			float f = i + .1f;
			iwecs::entity_t e = m.create_entity<int, float>(i, f);
		}

		m.destroy_entity(5);
		m.destroy_entity(85);
		m.destroy_entity(320);
		m.destroy_entity(199);

		for (int i = 0; i < 60; i++) {
			float f = i + .1f;
			iwecs::entity_t e = m.create_entity<int, float>(i, f);
		}

		m.create_entity<int, float>(2, 2.3f);

		intfloat_system ifs = intfloat_system();
		ifs.update();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}


//iwm	iwmath
//iwecs	iwecs
//iwevt	iwevents
//iwi	iwinput
//iwe	iwengine
//iwp	iwphysics
//iwa	iwaudio