#pragma once

#include "Layers/Menu_Layer.h"
#include "Events.h"
#include "Helpers.h"
#include "Weapons.h"
#include "MenuState.h"

#include "Components/CorePixels.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/graphics/ParticleSystem.h"

struct Menu_GameUI_Layer : Menu_Layer
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
		iw::SandLayer* sand_ui_laserCharge,
		iw::ref<iw::RenderTarget> bg
	)
		: Menu_Layer     ("Game UI")
		, m_sand_game     (sand)
		, m_sand_ui_laser (sand_ui_laserCharge)
		, m_player_score  (0)
		, m_player_weapon (nullptr)
		, m_paused        (false)
		, m_jitter        (0.f)
	{
		RegisterImage("bg", (void*)bg->Tex(0)->Handle()->Id());
	}

	int Initialize() override;
	void PreUpdate() override;
	void UI() override;

	bool On(iw::ActionEvent& e) override;
};
