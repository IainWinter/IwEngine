#pragma once

#include "Common.h"
#include "box2d/box2d.h"

struct Rigidbody2D
{
	Transform2D LastTransform;
	
	// Init values for Physics Add
	b2BodyDef m_body;
	b2FixtureDef m_collider;

	// If null, not in physics world
	b2Body* m_instance;

	Rigidbody2D()
		: m_instance (nullptr)
	{
		m_body.type = b2_dynamicBody;
	}
	
	bool InWorld() const
	{
		return !!m_instance;
	}
};

struct PhysicsWorld
{
	b2World* m_world;

	PhysicsWorld()
	{
		m_world = new b2World();
	}

	~PhysicsWorld()
	{
		delete m_world;
	}

	void Add(Rigidbody2D& body)
	{
		body.m_instance = m_world->CreateBody(&body.m_body);
		if (body.m_collider.density > 0)
		{
			body.m_instance->CreateFixture(&body.m_collider);
		}
	}
	
	void Remove(Rigidbody2D& body)
	{
		m_world->DestroyBody(body.m_instance);
		body.m_instance = nullptr;
	}

	void Step(float dt)
	{
		m_world->Step(dt, 8, 3);
	}

	// yes moves
	//  no copys
	PhysicsWorld(PhysicsWorld&& move)
		: m_world (move.m_world)
	{
		move.m_world = nullptr;
	}
	PhysicsWorld& operator=(PhysicsWorld&& move)
	{
		m_world = move.m_world;
		move.m_world = nullptr;
		return *this;
	}
	PhysicsWorld(const PhysicsWorld& move) = delete;
	PhysicsWorld& operator=(const PhysicsWorld& move) = delete;
};