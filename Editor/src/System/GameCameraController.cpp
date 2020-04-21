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
			target = m_target.FindComponent<iw::Transform>()->Position;
		}

		else {
			target = center;
		}

		target.y = 27.15f;

		if (transitionToCenter) {
			speed += 0.01f;

			if (   iw::almost_equal(t->Position.x, target.x, 2)
				&& iw::almost_equal(t->Position.z, target.z, 2))
			{
				Bus->push<AtNextLevelEvent>(); // should be timed

				t->Position.x -= target.x;
				t->Position.z -= target.z;
				
				center = 0;
				target = 0;

				transitionToCenter = false;
				speed = 2;
			}
		}

		iw::quaternion camrot =
			  iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
			* iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::Pi);

		t->Position = iw::lerp(t->Position, target, iw::Time::DeltaTime() * speed);
		t->Rotation = iw::lerp(t->Rotation, camrot, iw::Time::DeltaTime() * speed);

		//iw::DirectionalLight* sun = scene->DirectionalLights()[0];

		//iw::vector3 subpos = target;
		//subpos.y = 0;

		//sun->SetPosition(subpos);
	}
}

bool GameCameraController::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			follow = false;
			transitionToCenter = true;

			GoToNextLevelEvent& event = e.as<GoToNextLevelEvent>();

			center.x = event.CenterPosition.x;
			center.z = event.CenterPosition.y;

			//if (event.CameraFollow) {
			//	center.x += event.PlayerPosition.x;
			//	center.z += event.PlayerPosition.y;
			//}

			break;
		}
		case iw::val(Actions::START_LEVEL): {
			follow = e.as<StartLevelEvent>().CameraFollow;
			//transitionToCenter = !follow;

			break;
		}
		case iw::val(Actions::DEV_CONSOLE): {
			locked = !e.as<iw::ToggleEvent>().Active;
		}
	}

	return false;
}
