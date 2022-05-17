#include "../Entity.h"

struct vec2
{
	float x, y;
};

int main()
{
	entity_manager ent;
	ent.create().add<vec2>();
	ent.create().add<vec2>();
	entity e = ent.create().add<vec2>();

	vec2& v = e.get<vec2>();

	for (auto [e, i] : ent.query<vec2>())
	{
		
	}

	return 0;
}