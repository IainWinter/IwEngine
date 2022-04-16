#pragma once

#include "../Entity.h"

struct window
{
	
};

struct vec3
{
	float x, y, z;
};

struct rotor
{
	float x, y, z, w;
};

struct transform
{
	vec3 position;
	vec3 scale;
	rotor rotation; 
};

struct static_collider
{
	
};

struct static_body
{
	static_collider* collider;
};

struct static_physics
{

};

struct dynamic_physics
{

};


struct pos {};
struct vel {};

struct other {};

int main()
{
	entity application = entities().create<window, static_physics, dynamic_physics>();

	while (true)
	{
		for (auto [transform, body, what] : entities().query<transform, static_body>().only_change())
		{
			
		}
	} 



	entities().create<pos, vel>();
	entities().create<pos, vel>();
	entities().create<pos, vel, other>();
	entities().create<pos, vel, other>();
	
	
}

