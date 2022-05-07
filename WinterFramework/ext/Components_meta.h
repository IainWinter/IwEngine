#pragma once

#include "../Serial.h"

#include "Components.h"

inline
void register_component_types()
{
	meta::describe<NameComponent>()
		.name("Name Component")
		.member<&NameComponent::Name>("Name");

	meta::describe<Transform>()
		.name("Transform")
		.member<&Transform::Position>("Position")
		.member<&Transform::Rotation>("Rotation")
		.member<&Transform::Scale>   ("Scale");

	meta::describe<vec1b>().name("vec1b").member<&vec1b::x>("x");
	meta::describe<vec2b>().name("vec2b").member<&vec2b::x>("x").member<&vec2b::y>("y");
	meta::describe<vec3b>().name("vec3b").member<&vec3b::x>("x").member<&vec3b::y>("y").member<&vec3b::z>("z");
	meta::describe<vec4b>().name("vec4b").member<&vec4b::x>("x").member<&vec4b::y>("y").member<&vec4b::z>("z").member<&vec4b::w>("w");

	meta::describe<vec1i>().name("vec1i").member<&vec1i::x>("x");
	meta::describe<vec2i>().name("vec2i").member<&vec2i::x>("x").member<&vec2i::y>("y");
	meta::describe<vec3i>().name("vec3i").member<&vec3i::x>("x").member<&vec3i::y>("y").member<&vec3i::z>("z");
	meta::describe<vec4i>().name("vec4i").member<&vec4i::x>("x").member<&vec4i::y>("y").member<&vec4i::z>("z").member<&vec4i::w>("w");
	
	meta::describe<vec1>().name("vec1").member<&vec1::x>("x");
	meta::describe<vec2>().name("vec2").member<&vec2::x>("x").member<&vec2::y>("y");
	meta::describe<vec3>().name("vec3").member<&vec3::x>("x").member<&vec3::y>("y").member<&vec3::z>("z");
	meta::describe<vec4>().name("vec4").member<&vec4::x>("x").member<&vec4::y>("y").member<&vec4::z>("z").member<&vec4::w>("w");
}