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
	, follow(true)
	, speed(2.0)
{}

void GameCameraController::Update(
	iw::EntityComponentArray& eca)
{
	for (auto entity : eca) {
		auto [t, c] = entity.Components.Tie<Components>();

		if (follow) {
			iw::vector3 target = m_target.Find<iw::Transform>()->WorldPosition();
			target.y = y;

			t->Position = iw::lerp(t->Position, target, iw::DeltaTimeScaled() * 2);
		}

		else {
			t->Position = center;
		}

		previous = t->Position;
	}
}

bool GameCameraController::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::TRANSITION_TO_LEVEL): {
			TransitionToLevelEvent& event = e.as<TransitionToLevelEvent>();

			follow = event.CameraFollow;
			SetTarget(event.LevelName);

			if (!follow) {
				float start = iw::TotalTime();
				float wait = 1.0f;

				Task->queue([=]() {
					while (iw::ease(iw::TotalTime() - start) < wait) {
						center = iw::lerp(
							previous,
							iw::vector3(0, y, 0),
							iw::ease(iw::TotalTime() - start));
					}

					center = iw::vector3(0, y, 0);
				});
			}

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
	if (   levelName == "levels/forest/forest22.json"
		|| levelName.find("top")  != std::string::npos
		|| levelName.find("cave") != std::string::npos
		|| levelName.find(".a")   != std::string::npos
		|| levelName == "levels/canyon/canyon08.json"
		|| levelName == "levels/canyon/canyon09.json"
		|| levelName == "levels/canyon/canyon10.json")
	{
		y = 17.15f;
	}

	else {
		y = 27.15f;
	}
}
