#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "UI.h"
#include "Assets.h"
#include "Events.h"
#include "Helpers.h"

#include "Components/Weapon.h"
#include "Components/CorePixels.h"

struct Game_UI_Layer : iw::Layer
{
	iw::SandLayer* m_sand_game;
	iw::SandLayer* m_sand_ui_laser;

	CorePixels* m_player_core;
	Weapon* m_player_weapon;
	int m_cached_ammo;
	int m_cached_score;
	int m_player_score;

	float m_jitter;
	float m_offset;
	float m_bg_scale;
	bool m_game_over;
	bool m_game_paused;

	UIScreen* m_screen;
	UI* m_menu;
	UI* m_game;
	UI* m_health;
	UI* m_laser;
	UI* m_ammo;
	UI* m_score;
	UI* m_background;
	UI* m_version;

	iw::Camera* m_camera;

	Game_UI_Layer(
		iw::SandLayer* sand,
		iw::SandLayer* sand_ui_laserCharge
	)
		: iw::Layer       ("Game UI")

		, m_sand_game     (sand)
		, m_sand_ui_laser (sand_ui_laserCharge)

		, m_player_weapon (nullptr)
		, m_cached_ammo   (0)
		, m_cached_score  (0)
		, m_player_score  (0)

		, m_jitter        (0.f)
		, m_offset        (0.f)
		, m_bg_scale      (0.f)
		, m_game_over     (false)

		, m_screen        (nullptr)
		, m_menu          (nullptr)
		, m_game          (nullptr)
		, m_health        (nullptr)
		, m_laser         (nullptr)
		, m_ammo          (nullptr)
		, m_score         (nullptr)
		, m_background    (nullptr)
		, m_version       (nullptr)
	{
		m_camera = new iw::OrthographicCamera(2, 2, -10, 10);
		m_camera->Transform.Rotation = glm::angleAxis(iw::Pi, glm::vec3(0, 1, 0));
	}

	int Initialize() override;
	void PostUpdate() override;

	bool On(iw::ActionEvent& e) override;
};
