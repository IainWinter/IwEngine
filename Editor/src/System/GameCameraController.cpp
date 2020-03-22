#include "Systems/GameCameraController.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

GameCameraController::GameCameraController(iw::Entity& player)
	: System("Game Camera Controller")
	, player(player)
	, follow(true)
{}

void GameCameraController::Update(
	iw::EntityComponentArray& eca)
{
	for (auto entity : eca) {
		auto [t, c] = entity.Components.Tie<Components>();

		if (follow) {
			iw::vector3 target = player.FindComponent<iw::Transform>()->Position;
			t->Position.x = iw::lerp(t->Position.x, target.x, iw::Time::DeltaTime() * 2.0f);
			t->Position.z = iw::lerp(t->Position.z, target.z, iw::Time::DeltaTime() * 2.0f);
		}
	}
}

bool GameCameraController::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_NEXT_LEVEL): {
			follow = e.as<StartNextLevelEvent>().CameraFollow;

			for (auto entity : Space->Query<iw::Transform, iw::CameraController>()) {
				auto [t, c] = entity.Components.Tie<Components>();

				if (follow) {
					iw::vector3 target = player.FindComponent<iw::Transform>()->Position;
					t->Position.x = target.x;
					t->Position.z = target.y;
				}

				else {
					t->Position = iw::vector3(0, 27.15f, 0);
				}
			}

			break;
		}
	}

	return false;
}
