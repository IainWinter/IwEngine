#include "Systems/GameCameraController.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

GameCameraController::GameCameraController(
	iw::Entity& target,
	iw::Scene* scene)
	: System("Game Camera Controller")
	, m_target(target)
	, scene(scene)
	, locked(false)
	, follow(true)
	, transitionToCenter(false)
	, speed(2.0)
{}

float y = 27.15f;
float s = 1.0f;

bool zoomOut = false;

void GameCameraController::Update(
	iw::EntityComponentArray& eca)
{
	if (locked == true) {
		return;
	}

	for (auto entity : eca) {
		auto [t, c] = entity.Components.Tie<Components>();

		iw::vector3 target;
		if (follow) {
			target = m_target.Find<iw::Transform>()->Position;
		}

		else {
			target = center;
		}

		target.y = y;

		//if (   zoomOut
		//	&& target.x > -4)
		//{ // for 7.a
		//	target.y += 7;
		//}

		//target.x = 0;

		if (   !follow
			&& iw::almost_equal(t->Position.x, target.x, 2)
			&& iw::almost_equal(t->Position.y, target.y, 2)
			&& iw::almost_equal(t->Position.z, target.z, 2))
		{
			t->Position = target;
			s = speed;
		}

		else {
			//iw::quaternion camrot =
			//	iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
			//	* iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::Pi);

			if (!follow) {
				s += iw::Time::DeltaTimeScaled() * 5;
			}

			t->Position = iw::lerp(t->Position, target, iw::Time::DeltaTimeScaled() /** s*/);
			//t->Rotation = iw::lerp(t->Rotation, camrot, iw::Time::DeltaTime() * speed);
		}

		//iw::PointLight* lamp = scene->PointLights()[0];

		//iw::vector3 subpos = m_target.Find<iw::Transform>()->Position;
		//subpos.y = 1;

		//lamp->SetPosition(subpos);
	}
}

bool GameCameraController::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			follow = false;

			GoToNextLevelEvent& event = e.as<GoToNextLevelEvent>();

			center = 0;
			
			SetTarget(event.LevelName);
			follow = event.CameraFollow;
			s = speed;

			//if (event.CameraFollow) {
			//	center.x += event.PlayerPosition.x;
			//	center.z += event.PlayerPosition.y;
			//}

			break;
		}
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();

			SetTarget(event.LevelName);
			follow = event.CameraFollow;
			s = speed;

			break;
		}
		case iw::val(Actions::SET_CAMERA_TARGET): {
			SetCameraTargetEvent& event = e.as<SetCameraTargetEvent>();

			center = event.Target;
			y      = event.Target.y;
			
			if (event.ResetY) {
				SetTarget("");
			}

			break;
		}
		case iw::val(Actions::DEV_CONSOLE): {
			locked = !e.as<iw::ToggleEvent>().Active;

			break;
		}
	}

	return false;
}

void GameCameraController::SetTarget(
	std::string_view levelName)
{
	zoomOut = false;

	if (   levelName == "levels/forest/forest05.a.json"
		|| levelName == "levels/forest/forest07.a.json"
		|| levelName == "levels/forest/forest12.a.json"
		|| levelName == "levels/forest/forest22.json")
	{
		y = 17.15f;
	}

	/*else if(levelName == "levels/canyon/canyon02.json")
	{
		y = 22.15f;
	}*/

	else {
		y = 27.15f;
	}

	if (levelName == "levels/forest/forest07.a.json") {
		zoomOut = true;
	}
}
