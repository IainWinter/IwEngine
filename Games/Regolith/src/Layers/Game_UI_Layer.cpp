#include "Layers/Game_UI_Layer.h"

int Game_UI_Layer::Initialize()
{
	m_screen = new UIScreen();

	m_menu       = m_screen->CreateElement(A_mesh_ui_background);
	m_game       = m_screen->CreateElement(m_sand_game->GetSandMesh());
	m_health     = m_screen->CreateElement(A_mesh_ui_playerHealth);
	m_laser      = m_screen->CreateElement(m_sand_ui_laser->GetSandMesh());
	m_ammo       = m_screen->CreateElement(A_mesh_ui_text_ammo);
	m_score      = m_screen->CreateElement(A_mesh_ui_text_score);
	m_background = m_screen->CreateElement(A_mesh_background);
	m_version    = m_screen->CreateElement(A_mesh_ui_text_debug_version);

	return 0;
}

void Game_UI_Layer::PostUpdate()
{
	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	if (m_player_weapon)
	{
		int ammo = m_player_weapon->Ammo;

		if (m_cached_ammo != ammo)
		{
			m_cached_ammo = ammo;
			A_font_cambria->UpdateMesh(A_mesh_ui_text_ammo, ammo > 0 ? tos(ammo) : "inf", 5);
		}
	}

	if (m_cached_score != m_player_score)
	{
		m_cached_score = m_player_score;
		A_font_cambria->UpdateMesh(A_mesh_ui_text_score, tos(m_player_score), 5);
	}

	float red;

	if (m_game_over)
	{
		red = 0;
		m_jitter = 10;
	}

	else
	{
		float death = m_player_core->Timer / m_player_core->TimeWithoutCore;
		
		red = 1 - death;
		m_jitter = 75 * death;
		m_jitter = iw::lerp(m_jitter, 0.f, iw::DeltaTime() * 10);
	}

	A_mesh_ui_background  .Material->Set("color", iw::Color(1, red, red));
	A_mesh_ui_playerHealth.Material->Set("color", iw::Color(1, red, red));

	// screen - fills screen
	//		game board - fills empty space, 1:1 aspect ratio
	//		menu - y = 200px, x = width of game board
	//			m_player ui
	//			laser charges
	//			score
	//			time

	// constant constraints
	
	// independant vars in these eqs are

	// render width, render height, render depth
	// background_mesh
	// game sand world mesh
	// uiJitterAmount
	// uiBarOffset
	// ammo count
	// score count
	
	m_menu->z = 1;
	m_game->z = 0;

	m_menu->height = m_screen->height * .2f;                                    // Ratio(.2f)

	float uiBarOffsetTarget = 0;
	if (m_game_paused) uiBarOffsetTarget = -m_menu->height;
	else               uiBarOffsetTarget =  m_menu->height;
	m_offset = iw::lerp(m_offset, uiBarOffsetTarget, iw::DeltaTime() * 12);

	m_menu->x = iw::randf() * m_jitter;                               // Random(uiJitterAmount)
	m_menu->y = iw::randf() * m_jitter - m_screen->height + m_offset; //Combine(Anchor(BOTTOM), menu->height, Random(uiJitterAmount))

	m_game->y = m_menu->height;                      // Same(manu->height)
	m_game->height = m_screen->height - m_menu->height; // fill rest of screen y
	m_game->width  = m_game->height;                 // Same(game->height)

	m_menu->width = m_game->width; // Same(game->width)

	float menu_pad = m_menu->height * .2f;

	m_health->z = 2; 
	m_laser->z = -2; // ?? why does this have to be negitive

	m_health->height = m_menu->height - menu_pad;
	m_health->width = m_health->height; // Ratio(health->height)
	m_health->x = m_menu->x;            // Same(menu->x)
	m_health->y = m_menu->y;            // Same(menu->y)

	m_laser->height = m_menu->height - menu_pad;
	m_laser->width = m_laser->height;                               // Same (laser->height)
	m_laser->x = m_menu->x - m_menu->width + m_laser->width + menu_pad; // Combine(Anchor(LEFT), laser->width, menu_pads)
	m_laser->y = m_menu->y;

	float menu_pixel_x_scale = m_menu->width  / A_texture_ui_background->m_width;
	float menu_pixel_y_scale = m_menu->height / A_texture_ui_background->m_height;

	float ammo_x_pad  = (floor(m_cached_ammo  > 0 ? log10(m_cached_ammo)  : 1) * 11 + 30) * menu_pixel_x_scale;
	float score_x_pad = (floor(m_cached_score > 0 ? log10(m_cached_score) : 1) * 11 + 30) * menu_pixel_x_scale;

	float ammo_y_pad = 13 * menu_pixel_y_scale;
	float score_y_pad = 48 * menu_pixel_y_scale;


	m_ammo->width  = m_menu->height;
	m_ammo->height = m_menu->height; // Same(score->width)
	m_ammo->x = m_menu->x + m_menu->width - ammo_x_pad;
	m_ammo->y = m_menu->y + m_menu->height - ammo_y_pad;
	m_ammo->z = 2;

	m_score->width  = m_menu->height;
	m_score->height = m_menu->height; // Same(score->width)
	m_score->x = m_menu->x + m_menu->width - score_x_pad;
	m_score->y = m_menu->y + m_menu->height - score_y_pad;
	m_score->z = 2;

	if (m_game_over)
	{
		UI* gameover = m_screen->CreateElement(A_mesh_ui_text_gameOver);
		gameover->x = -m_screen->width*.8;
		gameover->y = m_screen->height * .5;
		gameover->width = m_screen->width;
		gameover->height = m_screen->width;
		gameover->z = 5;
	}

	float uiBackgroundScaleTarget = 0;
	if (m_game_paused) uiBackgroundScaleTarget = 3;
	else               uiBackgroundScaleTarget = 1;
	m_bg_scale = iw::lerp(m_bg_scale, uiBackgroundScaleTarget, iw::DeltaTime() * 12);

	m_background->y = m_game->y;
	m_background->width  = m_game->width  * m_bg_scale;
	m_background->height = m_game->height * m_bg_scale;
	m_background->z = -1;

	m_version->y = m_screen->height - 12;
	m_version->x = -m_screen->width + 1;
	m_version->width  = m_menu->width;
	m_version->height = m_menu->width;
	m_version->z = 5;

	m_screen->Draw(m_camera, Renderer);

}

bool Game_UI_Layer::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case PROJ_HIT_TILE:
		{
			ProjHitTile_Event& event = e.as<ProjHitTile_Event>();
			if (event.Config.Hit.Has<Player>())
			{
				m_jitter = 75;
			}

			break;
		}
		case END_STATE:
		{
			m_game_over = true;
			break;
		}
		case RUN_STATE: 
		{
			m_game_over = false;
			break;
		}
	}

	// listen for 
	// player switched weapon
	// changed score
	// game over
	// player hit
	// player healed
	// player creted core

	return false;
}
