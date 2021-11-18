#include "Layers/Game_UI_Layer.h"

int Game_UI_Layer::Initialize()
{
	m_screen = Space->CreateEntity<UI_Screen>().Set<UI_Screen>();

	// move out of assets
	A_mesh_ui_text_ammo     = A_font_cambria->GenerateMesh("0", { 100 });
	A_mesh_ui_text_score    = A_font_cambria->GenerateMesh("0", { 100 });
	A_mesh_ui_text_gameOver = A_font_cambria->GenerateMesh("YOU HAVE BEEN\n   DESTROYED", { 2, iw::FontAnchor::CENTER });

	{
		A_mesh_ui_text_debug_version = A_font_cambria->GenerateMesh("indev v.04", { 15, iw::FontAnchor::TOP_LEFT});
		A_mesh_ui_text_debug_version.Material = A_material_font_cam->MakeInstance();
		A_mesh_ui_text_debug_version.Material->Set("color", iw::Color(1, 1, 1, .25f));
		A_mesh_ui_text_debug_version.Material->Set("alphaThresh", .1f);
	}

	m_gameover   = m_screen->CreateElement(A_mesh_ui_text_gameOver);
	m_game       = m_screen->CreateElement(m_sand_game->GetSandMesh());
	m_menu       = m_screen->CreateElement(A_mesh_ui_background);
	m_health     = m_screen->CreateElement(A_mesh_ui_playerHealth);
	m_laser      = m_screen->CreateElement(m_sand_ui_laser->GetSandMesh());
	m_ammo       = m_screen->CreateElement(A_mesh_ui_text_ammo);
	m_score      = m_screen->CreateElement(A_mesh_ui_text_score);
	m_background = m_screen->CreateElement(A_mesh_background);
	m_version    = m_screen->CreateElement(A_mesh_ui_text_debug_version);

	m_background->zIndex = 0;
	m_game      ->zIndex = 1;
	m_menu      ->zIndex = 2;
	m_gameover  ->zIndex = 4;
	m_version   ->zIndex = 3;
	m_health    ->zIndex = 3; 
	m_laser     ->zIndex = 3;
	m_ammo      ->zIndex = 3;
	m_score     ->zIndex = 3;

	return 0;
}

void Game_UI_Layer::PreUpdate()
{
	m_sand_game->sP.x -= (m_screen->width - m_game->width) / 2.f / m_sand_game->m_cellSize;
	m_sand_game->sP.x *= (800.f / m_game->width);
	
	m_sand_game->sP.y = (m_sand_game->sP.y - 400) * (800.f / m_game->height) + 400;
}

int tick = 0;
std::unordered_map<iw::EntityHandle, std::pair<UI*, int>, iw::ehandle_hash> debug_tileColliders;

float fps = 0;

void Game_UI_Layer::PostUpdate()
{
	fps = iw::lerp(fps, iw::DeltaTime(), .01);

	std::stringstream ss;
	ss << "indev v.04 ms: ";
	ss << to_string(fps * 1000);

	A_font_cambria->UpdateMesh(A_mesh_ui_text_debug_version, ss.str(), {15, iw::FontAnchor::TOP_LEFT});

	iw::ref<iw::Texture> texture = m_game->mesh.Material->GetTexture("texture");

	// copy last 

	// could harden this update to not crash if something is null...

	if (m_player_weapon)
	{
		int ammo = m_player_weapon->Ammo;

		if (m_cached_ammo != ammo)
		{
			m_cached_ammo = ammo;
			A_font_cambria->UpdateMesh(A_mesh_ui_text_ammo, tos(ammo)/*ammo > 0 ? tos(ammo) : "0"*/, {100});
		}
	}

	if (m_cached_score != m_player_score)
	{
		m_cached_score = m_player_score;
		A_font_cambria->UpdateMesh(A_mesh_ui_text_score, tos(m_player_score), { 100 });
	}

	float red;

	if (m_game_over)
	{
		red = 0;
		m_jitter = 10;
	}

	else if (m_player_core)
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

	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	m_menu->height = m_screen->height * .2f;                                    // Ratio(.2f)

	float uiBarOffsetTarget = m_menu->height * (m_game_paused ? -1 : 1);
	m_offset = m_menu->height;//iw::lerp(m_offset, uiBarOffsetTarget, iw::DeltaTime() * 12);

	m_menu->x = iw::randf() * m_jitter;                               // Random(uiJitterAmount)
	m_menu->y = iw::randf() * m_jitter - m_screen->height + m_offset; //Combine(Anchor(BOTTOM), menu->height, Random(uiJitterAmount))

	m_game->y = m_menu->height;                      // Same(manu->height)
	m_game->height = m_screen->height - m_menu->height; // fill rest of screen y
	m_game->width  = m_game->height;                 // Same(game->height)

	m_menu->width = m_game->width; // Same(game->width)

	float menu_pad = m_menu->height * .2f;

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
	
	float ammo_x_pad  = (floor(m_cached_ammo  > 0 ? log10(m_cached_ammo)  : 0) * 11 + 30) * menu_pixel_x_scale;
	float score_x_pad = (floor(m_cached_score > 0 ? log10(m_cached_score) : 0) * 11 + 30) * menu_pixel_x_scale;

	float ammo_y_pad  = 13 * menu_pixel_y_scale;
	float score_y_pad = 48 * menu_pixel_y_scale;

	m_ammo->width  = m_menu->height;
	m_ammo->height = m_menu->height; // Same(score->width)
	m_ammo->x = m_menu->x + m_menu->width - ammo_x_pad;
	m_ammo->y = m_menu->y + m_menu->height - ammo_y_pad;

	m_score->width  = m_menu->height;
	m_score->height = m_menu->height; // Same(score->width)
	m_score->x = m_menu->x + m_menu->width - score_x_pad;
	m_score->y = m_menu->y + m_menu->height - score_y_pad;

	if (m_game_over)
	{
		auto [min, max] = m_gameover->mesh.GetBounds<iw::d2>();
		float x = max.x - min.x;
		float y = max.y - min.y;

		m_gameover->x = 0;//-x / 2 * m_game->width  + iw::randf() * m_jitter;
		m_gameover->y = 0;// y / 2 * m_game->height + iw::randf() * m_jitter;
		m_gameover->width  = m_game->width;
		m_gameover->height = m_game->width;
	}
	m_gameover->active = m_game_over;

	float uiBackgroundScaleTarget = m_game_paused ? 3 : 1;
	m_bg_scale = iw::lerp(m_bg_scale, uiBackgroundScaleTarget, iw::DeltaTime() * 12);

	m_background->y = m_game->y;
	m_background->width  = m_game->width  * m_bg_scale;
	m_background->height = m_game->height * m_bg_scale;

	m_version->y = m_screen->height - 12;
	m_version->x = -m_screen->width + 1;
	m_version->width  = m_menu->width;
	m_version->height = m_menu->width;


	tick++;

	Space->Query<iw::Tile>().Each([&](
		iw::EntityHandle entity, 
		iw::Tile* tile) 
	{
		if (!tile->m_collider.size() > 0)
		{
			return;
		}
				
		auto& [ui, mtick] = debug_tileColliders[entity];

		mtick = tick;

		if (ui)
		{
			// slow but works
			ui->mesh.Data->SetBufferData(iw::bName::POSITION, tile->m_collider.size(), tile->m_collider.data());
			ui->mesh.Data->SetIndexData(tile->m_colliderIndex.size(), tile->m_colliderIndex.data());

		}

		else
		{
			iw::MeshDescription desc;
			desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));

			iw::Mesh mesh;
			mesh = (new iw::MeshData(desc))->MakeInstance();
			mesh.Data->SetBufferData(iw::bName::POSITION, tile->m_collider.size(), tile->m_collider.data());
			mesh.Data->SetIndexData(tile->m_colliderIndex.size(), tile->m_colliderIndex.data());
			mesh.Material = A_material_debug_wireframe;

			ui = m_screen->CreateElement(mesh);
		}
	});

	for (auto& itr = debug_tileColliders.begin(); itr != debug_tileColliders.end(); itr++)
	{
		auto [mesh, mtick] = itr->second;
		if (mtick != tick)
		{
			m_screen->RemoveElement(itr->second.first);
			debug_tileColliders.erase(itr); // not sure if this is undefined
		}
	}

	for (auto& [entity, data] : debug_tileColliders)
	{
		iw::Transform* transform = Space->FindComponent<iw::Transform>(entity);
		UI* ui = data.first;

		ui->x = transform->Position.x;
		ui->y = transform->Position.y;
		ui->width = 4;
		ui->height = 4;
		ui->zIndex = 2;

		ui->transform.Rotation = transform->Rotation;
	}
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
		case CREATED_CORE_TILE:
		{
			CreatedCoreTile_Event& event = e.as<CreatedCoreTile_Event>();
			if (event.TileEntity.Has<Player>())
			{
				m_player_core = event.TileEntity.Find<CorePixels>();
			}

			break;
		}
		case STATE_CHANGE:
		{
			switch (e.as<StateChange_Event>().State)
			{
				case GAME_OVER_STATE:
				{
					m_game_over = true;
					break;
				}
				case GAME_START_STATE: 
				{
					m_game_over = false;
					break;
				}
				case GAME_PAUSE_STATE:
				{
					m_game_paused = true;
					break;
				}
				case GAME_RESUME_STATE: 
				{
					m_game_paused = false;
					break;
				}
			}

			break;
		}
		case CHANGE_PLAYER_WEAPON:
		{
			m_player_weapon = e.as<ChangePlayerWeapon_Event>().CurrentWeapon;
			break;
		}
		case CHANGE_PLAYER_SCORE:
		{
			m_player_score = e.as<ChangePlayerScore_Event>().Score;
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
