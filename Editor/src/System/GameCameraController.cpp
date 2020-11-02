#include "Systems/GameCameraController.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

GameCameraController::GameCameraController(
	iw::Entity& target)
	: System("Game Camera Controller")
	, m_target(target)
	, m_follow(true)
	, m_speed(2.0)
{}

void GameCameraController::Update(
	iw::EntityComponentArray& eca)
{
	for (auto entity : eca) {
		auto [t, c] = entity.Components.Tie<Components>();

		if (t->Parent() == nullptr) {
			t->SetParent(m_target.Find<iw::Transform>()->Parent());
		}

		if (m_follow) {
			iw::vector3 target = m_target.Find<iw::Transform>()->Position;
			target.y = m_y;

			t->Position = iw::lerp(t->Position, target, iw::DeltaTimeScaled() * m_speed);
		}

		else {
			t->Position = m_center;
		}

		m_previous = t->Position;
	}
}

bool GameCameraController::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::TRANSITION_TO_LEVEL): {
			TransitionToLevelEvent& event = e.as<TransitionToLevelEvent>();

			m_follow = false;
			SetTarget(event.LevelName);

			float start = iw::TotalTime();
			float wait = 1.5f;

			iw::vector3 target = event.CameraFollow
				? event.PlayerPosition
				: event.CenterPosition;

			Task->queue([=]() {
				while (iw::TotalTime() - start < wait) {
					m_center = iw::lerp(
						m_previous,
						iw::vector3(target.x, m_y, target.z),
						iw::ease(iw::TotalTime() - start, wait));
				}

				m_center = iw::vector3(target.x, m_y, target.z);
				m_follow = event.CameraFollow;
			});

			break;
		}
		case iw::val(Actions::SET_CAMERA_TARGET): {
			SetCameraTargetEvent& event = e.as<SetCameraTargetEvent>();

			m_center = event.Target;
			m_y      = event.Target.y;
			
			if (event.ResetY) {
				SetTarget("");
			}

			break;
		}
		case iw::val(Actions::DEV_CONSOLE): {
			m_locked = !e.as<iw::ToggleEvent>().Active;

			break;
		}
	}

	return false;
}

void GameCameraController::SetTarget(
	std::string_view levelName)
{
	if (   levelName == "levels/forest/forest22.json"
		|| levelName.find("top")   != std::string::npos
		|| levelName.find("cave")  != std::string::npos
		|| levelName.find("river") != std::string::npos
		|| levelName.find(".a")    != std::string::npos
		|| levelName == "levels/canyon/canyon08.json"
		|| levelName == "levels/canyon/canyon09.json"
		|| levelName == "levels/canyon/canyon10.json")
	{
		m_y = 17.15f;
	}

	else {
		m_y = 27.15f;
	}
}
