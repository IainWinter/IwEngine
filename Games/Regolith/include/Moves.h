#pragma once

#include "iw/engine/AppVars.h"
#include "plugins/iw/Sand/Cell.h"
#include "Events.h"
#include "glm/gtc/random.hpp"

struct Move
{
	virtual void Update(iw::AppVars app, entity player, bool use) = 0;
};

struct Dash_Move : Move
{
	float Speed = 1000;
	float Time  = 0.1f;
	float Timer = 0.f;
	float Cooldown = 1.f;
	glm::vec3 Move;

	void Update(iw::AppVars app, entity player, bool use) override
	{
		Timer -= iw::DeltaTime();

		glm::vec3 move = player.get<Player>().move;

		if (    use
			&& Timer < -Cooldown
			&& glm::length2(move) > 0.f)
		{
			Move = move;
			Timer = Time;
		}

		if (Timer >= 0.f)
		{
			player.get<iw::Rigidbody>().Velocity += Speed * Move * Timer / Time;
		}
	}
};

struct SmokeScreen_Move : Move
{
	float Time     = 0.5f;
	float Timer    = 0.f;
	float Cooldown = 5.f;

	void Update(iw::AppVars app, entity player, bool use) override
	{
		Timer -= iw::DeltaTime();

		if (    use
			&& Timer < -Cooldown)
		{
			Timer = Time;
		}

		if (Timer >= 0.f)
		{
			float x = player.get<iw::Transform>().Position.x;
			float y = player.get<iw::Transform>().Position.y;
			float life = 4;

			iw::Cell smoke = iw::Cell::GetDefault(iw::CellType::SMOKE);

			for (int i = 0; i < 100; i++)
			{
				glm::vec2 r = iw::norm(glm::vec2(iw::randfs(), iw::randfs())) * (30.f + 10.f * iw::randf());
				smoke.x = x + r.x;
				smoke.y = y + r.y;
				smoke.life = life + iw::randfs();

				app.Bus->push<SetCell_Event>(smoke);
			}
		}
	}
};

struct EnergyShield_Move : Move
{
	float Time     = 5.0f;
	float Timer    = 0.f;
	float Cooldown = 0.2f;
	bool LastFrame = false;

	void DrawShield(iw::AppVars app, float x, float y, float life, iw::Color color, float blend)
	{
		iw::Cell smoke = iw::Cell::GetDefault(iw::CellType::LIGHTNING);
		smoke.Props = iw::CellProp::MOVE_FORCE;

		if (color.a != 0)
		{
			glm::vec3 c = smoke.Color.rgb();
			c = iw::lerp(c, color.rgb(), blend);
			smoke.Color = iw::Color(c.x, c.y, c.z);
		}

		for (int i = 0; i < 100; i++)
		{
			glm::vec2 r = iw::norm(glm::vec2(iw::randfs(), iw::randfs())) * 30.f;
			smoke.x = x + r.x;
			smoke.y = y + r.y;
			smoke.dx = iw::randfs() * 100.f;
			smoke.dy = iw::randfs() * 100.f;
			smoke.life = life + iw::randfs() * life / 2.f;

			app.Bus->push<SetCell_Event>(smoke);
		}
	}

	void Update(iw::AppVars app, entity player, bool use) override
	{
		Timer -= iw::DeltaTime();

		if (    use
			&& Timer < -Cooldown)
		{
			Timer = Time;
		}

		bool& nodmg = player.get<Player>().NoDamage;

		nodmg = Timer >= 0.f;

		if (!nodmg && LastFrame)
		{
			iw::Rigidbody& playerBody = player.get<iw::Rigidbody>();

			float x = player.get<iw::Transform>().Position.x;
			float y = player.get<iw::Transform>().Position.y;
			float life = 1.5f;

			DrawShield(app, x, y, life, iw::Color::From255(255, 10, 75), 1.f);
		}

		LastFrame = nodmg;

		if (nodmg)
		{
			iw::Rigidbody& playerBody = player.get<iw::Rigidbody>();

			float x = player.get<iw::Transform>().Position.x;
			float y = player.get<iw::Transform>().Position.y;
			float life = .02;

			DrawShield(app, x, y, life, iw::Color::From255(255, 10, 75), 1.0f - Timer / Time);

			float shieldRadius = 30.f;
			float playerRadius = playerBody.Collider->as<iw::Circle>()->Radius;

			iw::DistanceQueryResult result = app.Physics->QueryPoint(glm::vec3(x, y, 0.f), shieldRadius);

			for (auto& [dist, obj] : result.Objects)
			{
				if (!obj->IsDynamic || &playerBody == obj)
					continue;

				iw::Rigidbody* body = (iw::Rigidbody*)obj;

				//if (app.Space->FindEntity(body).Has<Item>())
				//	continue;

				// not sure if this should affect your own bullets, might disable
				// and only enable with dust shield

				// replace with math that doesnt allow for anything inside of center

				glm::vec3 vel = body->Velocity + body->Transform.Position - glm::vec3(x, y, 0.f);
				body->Velocity = iw::norm(vel) * iw::clamp(glm::length(vel)/* * iw::DeltaTime() * 1000.f*/, 0.f, 1000.f);
			}
		}
	}
};
