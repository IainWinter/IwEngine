#include "Assets.h"

using namespace iw;

int LoadAssets(
	iw::AssetManager* Asset,
	iw::Renderer* Renderer)
{
	A_texture_player         = Asset->Load<Texture>("textures/SpaceGame/player.png");
	A_texture_enemy1         = Asset->Load<Texture>("textures/SpaceGame/enemy.png");
	A_texture_background     = Asset->Load<Texture>("textures/SpaceGame/space.png");
	A_texture_asteroid_mid_1 = Asset->Load<Texture>("textures/SpaceGame/asteroid_mid_1.png");
	A_texture_asteroid_mid_2 = Asset->Load<Texture>("textures/SpaceGame/asteroid_mid_2.png");
	A_texture_asteroid_mid_3 = Asset->Load<Texture>("textures/SpaceGame/asteroid_mid_3.png");
	A_texture_item_health    = Asset->Load<Texture>("textures/SpaceGame/item_health.png");
	A_texture_item_energy    = Asset->Load<Texture>("textures/SpaceGame/item_energy.png");
	A_texture_item_minigun   = Asset->Load<Texture>("textures/SpaceGame/item_minigun.png");
	A_texture_item_coreShard = Asset->Load<Texture>("textures/SpaceGame/item_coreShard.png");
	A_texture_ui_cursor      = Asset->Load<Texture>("textures/SpaceGame/cursor.png");
	A_texture_ui_background  = Asset->Load<Texture>("textures/SpaceGame/ui_background.png");
	A_texture_font_arial     = Asset->Load<Texture>("textures/fonts/arial.png");

	A_font_arial = Asset->Load<iw::Font>("fonts/arial.fnt");
	
	A_material_texture_cam = REF<Material>(Asset->Load<Shader>("shaders/texture_cam.shader"));
	A_material_font_cam    = REF<Material>(Asset->Load<Shader>("shaders/font.shader"));

	A_mesh_background      = iw::ScreenQuad().MakeInstance();
	A_mesh_ui_background   = iw::ScreenQuad().MakeInstance();
	A_mesh_ui_playerHealth = iw::ScreenQuad().MakeInstance();
	
#define CHECK_LOAD(x) if(!x) { LOG_ERROR << "Failed to load " << #x; return 100; }
	
	CHECK_LOAD(A_texture_player        );
	CHECK_LOAD(A_texture_enemy1        );
	CHECK_LOAD(A_texture_background    );
	CHECK_LOAD(A_texture_asteroid_mid_1);
	CHECK_LOAD(A_texture_asteroid_mid_2);
	CHECK_LOAD(A_texture_asteroid_mid_3);
	CHECK_LOAD(A_texture_item_health   );
	CHECK_LOAD(A_texture_item_energy   );
	CHECK_LOAD(A_texture_item_minigun  );
	CHECK_LOAD(A_texture_item_coreShard);
	CHECK_LOAD(A_texture_ui_cursor     );
	CHECK_LOAD(A_texture_ui_background );
	CHECK_LOAD(A_texture_font_arial    );

	CHECK_LOAD(A_font_arial);

	CHECK_LOAD(A_material_texture_cam->Shader);
	CHECK_LOAD(A_material_font_cam   ->Shader);

	A_texture_player        ->m_filter = NEAREST;
	A_texture_enemy1        ->m_filter = NEAREST;
	A_texture_background    ->m_filter = NEAREST;
	A_texture_asteroid_mid_1->m_filter = NEAREST;
	A_texture_asteroid_mid_2->m_filter = NEAREST;
	A_texture_asteroid_mid_3->m_filter = NEAREST;
	A_texture_item_health   ->m_filter = NEAREST;
	A_texture_item_energy   ->m_filter = NEAREST;
	A_texture_item_minigun  ->m_filter = NEAREST;
	A_texture_item_coreShard->m_filter = NEAREST;
	A_texture_ui_cursor     ->m_filter = NEAREST;
	A_texture_ui_background ->m_filter = NEAREST;
	A_texture_font_arial    ->m_filter = NEAREST;

	A_texture_player        ->m_wrap = EDGE;
	A_texture_enemy1        ->m_wrap = EDGE;
	A_texture_background    ->m_wrap = EDGE;
	A_texture_asteroid_mid_1->m_wrap = EDGE;
	A_texture_asteroid_mid_2->m_wrap = EDGE;
	A_texture_asteroid_mid_3->m_wrap = EDGE;
	A_texture_item_health   ->m_wrap = EDGE;
	A_texture_item_energy   ->m_wrap = EDGE;
	A_texture_item_minigun  ->m_wrap = EDGE;
	A_texture_item_coreShard->m_wrap = EDGE;
	A_texture_ui_cursor     ->m_wrap = EDGE;
	A_texture_ui_background ->m_wrap = EDGE;
	A_texture_font_arial    ->m_wrap = EDGE;

	{
		Renderer->InitShader(A_material_texture_cam->Shader, CAMERA);
		A_material_texture_cam->Set("alphaThresh", .6f);
		A_material_texture_cam->Set("color", iw::Color(1.f));
	}

	{
		Renderer->InitShader(A_material_font_cam->Shader, iw::CAMERA);
		A_material_font_cam->SetTransparency(Transparency::ADD);
		A_material_font_cam->SetTexture("fontMap", A_texture_font_arial);
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
		ui_player_texture->SetFilter(iw::NEAREST);
		ui_player_texture->CreateColors();

		A_mesh_ui_playerHealth.Material = A_material_texture_cam->MakeInstance();
		A_mesh_ui_playerHealth.Material->SetTexture("texture", ui_player_texture);
	}

	{
		A_mesh_ui_text_ammo = A_font_arial->GenerateMesh("0", .001f, 1);
		A_mesh_ui_text_ammo.Material = A_material_font_cam->MakeInstance();
	}

	{
		A_mesh_ui_text_gameOver = A_font_arial->GenerateMesh("GAME OVER\npress space to restart...", .001f, 1);
		A_mesh_ui_text_gameOver.Material = A_material_font_cam->MakeInstance();
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
