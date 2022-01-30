#include "Layers/Game_UI_Layer.h"

int Menu_GameUI_Layer::Initialize()
{
	RegisterImage("ui_background.png");
	imgs["game"] = (void*)m_sand_game    ->GetSandTextureId();
	imgs["tank"] = (void*)m_sand_ui_laser->GetSandTextureId();
	imgs["player"] = (void*)A_mesh_ui_playerHealth.Material->GetTexture("texture")->Handle()->Id();

	m_playerImgWidth = A_mesh_ui_playerHealth.Material->GetTexture("texture")->Width();

	return 0;
}

void Menu_GameUI_Layer::UI()
{
	float ui_h = bg_h * .2f;
	float ui_y = bg_h - ui_h;

	float scale = bg_w / 200.f;

	//float red;

	//if (m_game_over)
	//{
	//	red = 0;
	//	m_jitter = 10;
	//}

	//else if (m_player_core)
	//{
	//	float death = m_player_core->Timer / m_player_core->TimeWithoutCore;

	//	red = 1 - death;
	//	m_jitter = 75 * death;
		m_jitter = iw::lerp(m_jitter, 0.f, iw::DeltaTime() * 10);
	//}

	// Draw the main ui

	ImGui::PushFont(iwFont("Quicksand_50"));
	ImGui::SetNextWindowPos (ImVec2(bg_x + iw::randf() * m_jitter, ui_y - iw::randf() * m_jitter));
	ImGui::SetNextWindowSize(ImVec2(bg_w, ui_h));
	ImGui::Begin("Main Game UI", nullptr, commonFlags);
	{
		// background

		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(imgs["ui_background.png"], ImVec2(bg_w, ui_h));
		
		// player

		float player_wh = 40 * scale;
		float player_x  = 80 * scale;
		float player_y  = 5 * scale;

		ImGui::SetCursorPos(ImVec2(player_x, player_y));
		ImGui::Image(imgs["player"], ImVec2(player_wh, player_wh));

		// ammo and score text
		// could make this on exact center with a ImGui::TextSize calculation

		float text_x  = 146  * scale;
		float ammo_y  = 4.5f * scale;
		float score_y = 22.f * scale;

		if (m_player_weapon && m_player_weapon->Ammo > 0)
		{
			ImGui::SetCursorPos(ImVec2(text_x, ammo_y));
			ImGui::Text("%d", m_player_weapon->Ammo);
		}
		
		ImGui::SetCursorPos(ImVec2(text_x, score_y));
		ImGui::Text("%d", m_player_score);

		// particles for hit animation

		for (int i = 0; i < m_hitParticles.size(); i++)
		{
			HitParticle& hit = m_hitParticles.at(i);

			std::vector<glm::vec2> pos =
			{
				glm::vec2(-1, -1),
				glm::vec2( 1, -1),
				glm::vec2( 1,  1),
				glm::vec2(-1,  1)
			};

			float pixelScale = player_wh / m_playerImgWidth;

			iw::Transform trans;
			trans.Position = glm::vec3(bg_x + player_x, ui_y + player_y, 0.f) + (hit.Pos + glm::vec3(.5, -.5, 0)) * pixelScale;
			trans.Rotation = glm::quat(glm::vec3(0, 0, hit.Life * hit.AngleScale));
			trans.Scale = glm::vec3(pixelScale / 2.f);

			iw::TransformPolygon(pos, &trans);

			ImGui::GetForegroundDrawList()->AddQuadFilled(
				ImVec2(pos[0].x, pos[0].y), 
				ImVec2(pos[1].x, pos[1].y),
				ImVec2(pos[2].x, pos[2].y),
				ImVec2(pos[3].x, pos[3].y), hit.Color.to32());

			if (!m_paused)
			{
				hit.Pos += hit.Vel;
				hit.Color.a = (hit.Lifetime - hit.Life) / hit.Lifetime;
				hit.Life += iw::DeltaTime();
				if (hit.Life > hit.Lifetime)
				{
					hit = m_hitParticles.back();
					m_hitParticles.pop_back();
					i--;
				}
			}
		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos (ImVec2(bg_x, bg_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h - ui_h));
	ImGui::Begin("Main Game Board", nullptr, commonFlags);
	{
		ImGui::Image(imgs["game"], ImVec2(bg_w, bg_h - ui_h));
	}
	ImGui::End();
	ImGui::PopFont();
}

void Menu_GameUI_Layer::PreUpdate()
{
	m_sand_game->sP.x -= (window_w - bg_w) / 2.f / m_sand_game->m_cellSize;
	m_sand_game->sP.x *= (800.f / bg_w);
	
	m_sand_game->sP.y = (m_sand_game->sP.y - 400) * (800.f / bg_w) + 400; // should be game_h but its a square based on bg_w
}

int tick = 0;
std::unordered_map<iw::EntityHandle, std::pair<UI*, int>, iw::ehandle_hash> debug_tileColliders;

float fps = 0;

void Game_UI_Layer::PostUpdate()
{
	//fps = iw::lerp(fps, iw::DeltaTime(), .01);
	fps = iw::DeltaTime();

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

	//m_background->y = m_game->y;
	//m_background->width  = m_game->width  * m_bg_scale;
	//m_background->height = m_game->height * m_bg_scale;

	m_version->y = m_screen->height - 12;
	m_version->x = -m_screen->width + 1;
	m_version->width  = m_menu->width;
	m_version->height = m_menu->width;

	//tick++;

	//Space->Query<iw::Tile>().Each([&](
	//	iw::EntityHandle entity, 
	//	iw::Tile* tile) 
	//{
	//	if (!tile->m_collider.size() > 0)
	//	{
	//		return;
	//	}
	//			
	//	auto& [ui, mtick] = debug_tileColliders[entity];

	//	mtick = tick;

	//	if (ui)
	//	{
	//		// slow but works
	//		ui->mesh.Data->SetBufferData(iw::bName::POSITION, tile->m_collider.size(), tile->m_collider.data());
	//		ui->mesh.Data->SetIndexData(tile->m_colliderIndex.size(), tile->m_colliderIndex.data());

	//	}

	//	else
	//	{
	//		iw::MeshDescription desc;
	//		desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));

	//		iw::Mesh mesh;
	//		mesh = (new iw::MeshData(desc))->MakeInstance();
	//		mesh.Data->SetBufferData(iw::bName::POSITION, tile->m_collider.size(), tile->m_collider.data());
	//		mesh.Data->SetIndexData(tile->m_colliderIndex.size(), tile->m_colliderIndex.data());
	//		mesh.Material = A_material_debug_wireframe;

	//		ui = m_screen->CreateElement(mesh);
	//	}
	//});

	//for (auto& itr = debug_tileColliders.begin(); itr != debug_tileColliders.end(); itr++)
	//{
	//	auto [mesh, mtick] = itr->second;
	//	if (mtick != tick)
	//	{
	//		m_screen->RemoveElement(itr->second.first);
	//		debug_tileColliders.erase(itr); // not sure if this is undefined
	//	}
	//}

	//for (auto& [entity, data] : debug_tileColliders)
	//{
	//	iw::Transform* transform = Space->FindComponent<iw::Transform>(entity);
	//	UI* ui = data.first;

	//	ui->x = transform->Position.x;
	//	ui->y = transform->Position.y;
	//	ui->width = 4;
	//	ui->height = 4;
	//	ui->zIndex = 2;

	//	ui->transform.Rotation = transform->Rotation;
	//}
}

bool Menu_GameUI_Layer::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case PROJ_HIT_TILE:
		{
			ProjHitTile_Event& event = e.as<ProjHitTile_Event>();
			if (event.Config.Hit.Has<Player>())
			{
				int index  = event.Config.Info.index;
				int width  = event.Config.Info.tile->m_sprite.m_width;
				int height = event.Config.Info.tile->m_sprite.m_height;

				auto [x, y] = iw::xy(index, width);
				iw::Color color = iw::Color::From32(event.Config.Info.tile->m_sprite.Colors32()[index]);
				glm::vec3 vel = event.Config.Projectile.Find<iw::Rigidbody>()->Velocity;

				color.a = 1.f;
				vel.y *= -1;

				HitParticle particle;
				particle.Life = 0.f;
				particle.Lifetime = 1.f;
				particle.AngleScale = iw::randfs() * 10;
				particle.Color = color;
				particle.Pos = glm::vec3(x, height - y, 0.f);
				particle.Vel = glm::normalize(vel) * .05f * (iw::randf() + 1);

				m_hitParticles.push_back(particle);

				m_jitter = 45;
			}

			break;
		}
		case STATE_CHANGE:
		{
			switch (e.as<StateChange_Event>().State)
			{
				case StateName::IN_GAME:
				{
					m_paused = false;
					break;
				}
				case StateName::IN_GAME_THEN_MENU:
				{
					m_paused = true;
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

	return false;
}
