#pragma once

#include "Layers/Menu_Layer.h"
#include "Events.h"
#include "Helpers.h"
#include "Weapons.h"

#include "Components/CorePixels.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/graphics/ParticleSystem.h"

struct Menu_GameUI_Layer : Menu_Layer2
{
	iw::SandLayer* m_sand_game;
	iw::SandLayer* m_sand_ui_laser;

	Weapon* m_player_weapon;
	int m_player_score;

	float m_jitter;

	bool m_paused;
	int m_playerImgWidth;

	struct HitParticle
	{
		float Life;
		float Lifetime;
		float AngleScale;

		iw::Color Color;

		glm::vec3 Vel;
		glm::vec3 Pos;
	};

	std::vector<HitParticle> m_hitParticles;

	Menu_GameUI_Layer(
		iw::SandLayer* sand,
		iw::SandLayer* sand_ui_laserCharge
	)
		: Menu_Layer2     ("Game UI")
		, m_sand_game     (sand)
		, m_sand_ui_laser (sand_ui_laserCharge)
		, m_player_score  (0)
		, m_player_weapon (nullptr)
		, m_paused        (false)
		, m_jitter        (0.f)
	{}

	int Initialize() override;
	void PreUpdate() override;
	void UI() override;

	bool On(iw::ActionEvent& e) override;
};

struct Game_UI_Layer : UI_Layer
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

	UI* m_menu;
	UI* m_game;
	UI* m_health;
	UI* m_laser;
	UI* m_ammo;
	UI* m_score;
	UI* m_background;
	UI* m_version;
	UI* m_gameover;

	Game_UI_Layer(
		iw::SandLayer* sand,
		iw::SandLayer* sand_ui_laserCharge
	)
		: UI_Layer      ("Game UI")

		, m_sand_game     (sand)
		, m_sand_ui_laser (sand_ui_laserCharge)

		, m_player_core   (nullptr)
		, m_player_weapon (nullptr)
		, m_cached_ammo   (0)
		, m_cached_score  (0)
		, m_player_score  (0)

		, m_jitter        (0.f)
		, m_offset        (0.f)
		, m_bg_scale      (1.f)
		, m_game_over     (false)
		, m_game_paused   (false)

		, m_menu          (nullptr)
		, m_game          (nullptr)
		, m_health        (nullptr)
		, m_laser         (nullptr)
		, m_ammo          (nullptr)
		, m_score         (nullptr)
		, m_background    (nullptr)
		, m_version       (nullptr)
	{}


	int Initialize() override;
	void PreUpdate() override;
	void PostUpdate() override;

	bool On(iw::ActionEvent& e) override;
};
