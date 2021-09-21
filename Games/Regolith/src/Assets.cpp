#include "Assets.h"

using namespace iw;

int LoadAssets(
	iw::AssetManager* Asset,
	iw::Renderer* Renderer)
{

	std::vector<ref<Texture>> textures;

#define LOADTEX(x) textures.emplace_back(Asset->Load<Texture>(x))

	A_texture_player         = LOADTEX("textures/SpaceGame/player.png");
	A_texture_enemy_fighter  = LOADTEX("textures/SpaceGame/enemy_fighter.png");
	A_texture_enemy_bomb     = LOADTEX("textures/SpaceGame/enemy_bomb.png");
	A_texture_enemy_station  = LOADTEX("textures/SpaceGame/enemy_station.png");
	A_texture_background     = LOADTEX("textures/SpaceGame/space.png");
	A_texture_asteroid_mid_1 = LOADTEX("textures/SpaceGame/asteroid_mid_1.png");
	A_texture_asteroid_mid_2 = LOADTEX("textures/SpaceGame/asteroid_mid_2.png");
	A_texture_asteroid_mid_3 = LOADTEX("textures/SpaceGame/asteroid_mid_3.png");
	A_texture_item_health    = LOADTEX("textures/SpaceGame/item_health.png");
	A_texture_item_energy    = LOADTEX("textures/SpaceGame/item_energy.png");
	A_texture_item_minigun   = LOADTEX("textures/SpaceGame/item_minigun.png");
	A_texture_item_coreShard = LOADTEX("textures/SpaceGame/item_coreShard.png");
	A_texture_ui_cursor      = LOADTEX("textures/SpaceGame/cursor.png");
	A_texture_ui_background  = LOADTEX("textures/SpaceGame/ui_background.png");
	A_texture_font_cambria   = LOADTEX("textures/fonts/cambria_lowres.png");

	A_font_cambria = Asset->Load<iw::Font>("fonts/cambria_lowres.fnt");

	A_material_texture_cam = REF<Material>(Asset->Load<Shader>("shaders/texture_cam.shader"));
	A_material_font_cam    = REF<Material>(Asset->Load<Shader>("shaders/font.shader"));

	A_mesh_background      = iw::ScreenQuad().MakeInstance();
	A_mesh_ui_background   = iw::ScreenQuad().MakeInstance();
	A_mesh_ui_playerHealth = iw::ScreenQuad().MakeInstance();
	A_mesh_menu_pause      = iw::ScreenQuad().MakeInstance();
	A_mesh_menu_background = iw::ScreenQuad().MakeInstance();

#define CHECK_LOAD(x) if(!x) { LOG_ERROR << "Failed to load " << #x; return 100; }
	
	for (ref<Texture>& tex : textures)
	{
		CHECK_LOAD(tex);
		tex->m_filter = NEAREST;
		tex->m_wrap = EDGE;
	}

	CHECK_LOAD(A_font_cambria);
	CHECK_LOAD(A_material_texture_cam->Shader);
	CHECK_LOAD(A_material_font_cam   ->Shader);

	{
		Renderer->InitShader(A_material_texture_cam->Shader, CAMERA);
		A_material_texture_cam->SetTransparency(Transparency::ADD);
		A_material_texture_cam->Set("alphaThresh", .6f);
		A_material_texture_cam->Set("color", iw::Color(1.f));
	}

	{
		Renderer->InitShader(A_material_font_cam->Shader, iw::CAMERA);
		A_material_font_cam->SetTransparency(Transparency::ADD);
		A_material_font_cam->SetTexture("fontMap", A_texture_font_cambria);
		A_material_font_cam->Set("color", iw::Color(1));
	}

	{
		A_mesh_background.Material = A_material_texture_cam->MakeInstance();
		A_mesh_background.Material->SetTexture("texture", A_texture_background);
	}

	{
		A_mesh_ui_background.Material = A_material_texture_cam->MakeInstance();
		A_mesh_ui_background.Material->SetTexture("texture", A_texture_ui_background);
	}

	{
		ref<Texture> ui_player_texture = REF<Texture>(*A_texture_player);
		ui_player_texture->CreateColors();

		A_mesh_ui_playerHealth.Material = A_material_texture_cam->MakeInstance();
		A_mesh_ui_playerHealth.Material->SetTexture("texture", ui_player_texture);
	}

	{
		A_mesh_ui_text_ammo = A_font_cambria->GenerateMesh("0", 4);
		A_mesh_ui_text_ammo.Material = A_material_font_cam;
	}

	{
		A_mesh_ui_text_score = A_font_cambria->GenerateMesh("0", 4);
		A_mesh_ui_text_score.Material = A_material_font_cam;
	}

	{
		A_mesh_ui_text_gameOver = A_font_cambria->GenerateMesh("GAME OVER\npress space to restart...", 2);
		A_mesh_ui_text_gameOver.Material = A_material_font_cam;
	}

	{
		A_mesh_ui_text_debug_version = A_font_cambria->GenerateMesh("indev v.04", 1);
		A_mesh_ui_text_debug_version.Material = A_material_font_cam->MakeInstance();
		A_mesh_ui_text_debug_version.Material->Set("color", iw::Color(1, 1, 1, .25));
	}

	// Menus

	{
		A_mesh_menu_pause_title = A_font_cambria->GenerateMesh("Pause Menu\n\ncoming soon...", 2);
		A_mesh_menu_pause_title.Material = A_material_font_cam;
	}

	{
		A_mesh_menu_background.Material = A_material_texture_cam->MakeInstance();
		A_mesh_menu_background.Material->Set("alphaThresh", 0.0f);
		A_mesh_menu_background.Material->Set("color", iw::Color(.16, .16, .16, .5));
		A_mesh_menu_background.Material->SetTransparency(Transparency::ADD);
	}

	{
		A_mesh_menu_pause.Material = A_material_texture_cam->MakeInstance();
		A_mesh_menu_pause.Material->Set("alphaThresh", 0.0f);
		A_mesh_menu_pause.Material->Set("color", iw::Color(.5));
		A_mesh_menu_pause.Material->SetTransparency(Transparency::ADD);
	}

	return 0;
}


//A_texture_star           = Asset->Load<Texture>("textures/SpaceGame/star.png");
//A_texture_asteroid       = Asset->Load<Texture>("textures/SpaceGame/asteroid.png");
//A_material_texture_cam_particle = REF<Material>(Asset->Load<Shader>("shaders/particle/texture_cam.shader"));
//A_material_debug_wireframe      = REF<Material>(Asset->Load<Shader>("shaders/debug/wireframe2_cam.shader"));
//A_mesh_star            = iw::ScreenQuad().MakeCopy();
//A_mesh_ui_cursor        = iw::ScreenQuad().MakeInstance();

//CHECK_LOAD(A_texture_star 		   )
//CHECK_LOAD(A_texture_asteroid	   )
//CHECK_LOAD(A_material_texture_cam_particle->Shader)
//CHECK_LOAD(A_material_debug_wireframe     ->Shader)
//A_texture_star          ->m_filter = NEAREST;
//A_texture_asteroid      ->m_filter = NEAREST;
//A_texture_star          ->m_wrap = EDGE;
//A_texture_asteroid      ->m_wrap = EDGE;

//{
//	Renderer->InitShader(A_material_texture_cam_particle->Shader, CAMERA);
//}

//{
//	Renderer->InitShader(A_material_debug_wireframe->Shader, CAMERA);
//	A_material_debug_wireframe->Set("color", iw::Color(1, 0, 0));
//	A_material_debug_wireframe->SetWireframe(true);
//}

//{
//	A_mesh_ui_cursor.Material = A_material_texture_cam->MakeInstance();
//	A_mesh_ui_cursor.Material->SetTexture("texture", A_texture_ui_cursor);
//}
//{
//	A_mesh_star.Material = A_material_texture_cam_particle->MakeInstance();
//	A_mesh_star.Material->SetTexture("texture", A_texture_star);
//	A_mesh_star.Material->Set("useAlpha", 1);
//}
