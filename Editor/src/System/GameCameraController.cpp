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

float startTime2 = 0;
bool firstTransition = false;
iw::vector3 lastPosition = 0;

bool zoomOut = false;
bool transition2 = false;

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

		if (firstTransition) {
			lastPosition = t->Position;
			firstTransition = false;
			transition2 = true;
		}

		else if (transition2) {
			float time = 1.0f * (iw::Time::TotalTime() - startTime2);

			t->Position = iw::lerp(lastPosition, target, time);

			//iw::quaternion camrot =
			//	iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
			//	* iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::Pi);
			//t->Rotation = iw::lerp(t->Rotation, camrot, iw::Time::DeltaTime() * speed);

			if (time > 1) {
				t->Position = target;
				transition2 = false;
			}
		}

		else {
			t->Position = iw::lerp(t->Position, target, iw::DeltaTimeScaled() * 2);
		}
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
			follow = event.CameraFollow;

			startTime2 = iw::TotalTime();
			firstTransition = true;

			SetTarget(event.LevelName);

			break;
		}
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();

			follow = event.CameraFollow;

			SetTarget(event.LevelName);

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

	if (//   levelName == "levels/forest/forest05.a.json"
		//|| levelName == "levels/forest/forest07.a.json"
		//|| levelName == "levels/forest/forest12.a.json"
		/*||*/ 
		   levelName == "levels/forest/forest22.json"
		|| levelName.find("top")  != std::string::npos
		|| levelName.find("cave") != std::string::npos
		|| levelName.find(".a") != std::string::npos
		|| levelName == "levels/canyon/canyon08.json"
		|| levelName == "levels/canyon/canyon09.json"
		|| levelName == "levels/canyon/canyon10.json")
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
