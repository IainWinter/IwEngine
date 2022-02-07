#include "Layers/Game_UI_Layer.h"

int Menu_GameUI_Layer::Initialize()
{
	RegisterImage("ui_background.png");
	RegisterImage("game", m_sand_game    ->GetSandTextureId());
	RegisterImage("tank", m_sand_ui_laser->GetSandTextureId());
	RegisterImage("player", (void*)A_mesh_ui_playerHealth.Material->GetTexture("texture")->Handle()->Id());

	m_playerImgWidth = A_mesh_ui_playerHealth.Material->GetTexture("texture")->Width();

	return 0;
}

void Menu_GameUI_Layer::UI()
{
	float ui_h = floor(bg_h * .2f);
	float ui_y = bg_h - ui_h - iw::randf() * m_jitter;

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
	ImGui::SetNextWindowPos (ImVec2(bg_x, ui_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, ui_h));
	ImGui::Begin("Main Game UI", nullptr, commonFlagsFocus);
	{
		// bg

		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(Image("bg"), ImVec2(bg_w, bg_h), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();

		// game

		//ImGui::SetCursorPos(ImVec2(0, 0));
		//ImGui::Image(Image("game"), ImVec2(bg_w, bg_h - ui_h));

		// ui 

		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(Image("ui_background.png"), ImVec2(bg_w, ui_h));
		
		// player

		float player_wh = 40 * scale;
		float player_x  = 80 * scale;
		float player_y  =  5 * scale;

		ImGui::SetCursorPos(ImVec2(player_x, player_y));
		ImGui::Image(Image("player"), ImVec2(player_wh, player_wh));

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
		ImGui::Image(Image("game"), ImVec2(bg_w, bg_h - ui_h));
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

bool Menu_GameUI_Layer::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case PROJ_HIT_TILE:
		{
			ProjHitTile_Event& event = e.as<ProjHitTile_Event>();
			if (   event.Config.Hit.Has<Player>()
				&& event.Config.Projectile.Alive()) // for crash ?
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


//int tick = 0;
//std::unordered_map<iw::EntityHandle, std::pair<UI*, int>, iw::ehandle_hash> debug_tileColliders;
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
