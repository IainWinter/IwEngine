#include "Assets.h"

using namespace iw;

int LoadAssets(
	iw::AssetManager* Asset,
	iw::Renderer* Renderer)
{
	A_texture_enemy1        = Asset->Load<Texture>("textures/SpaceGame/enemy.png");
	A_texture_player        = Asset->Load<Texture>("textures/SpaceGame/player.png");
	A_texture_star          = Asset->Load<Texture>("textures/SpaceGame/star.png");
	A_texture_asteroid      = Asset->Load<Texture>("textures/SpaceGame/asteroid.png");
	A_texture_ui_cursor     = Asset->Load<Texture>("textures/SpaceGame/circle_temp.png");
	A_texture_ui_background = Asset->Load<Texture>("textures/SpaceGame/ui_background.png");
	A_texture_item_health   = Asset->Load<Texture>("textures/SpaceGame/item_health.png");
	A_texture_item_energy   = Asset->Load<Texture>("textures/SpaceGame/item_energy.png");
	A_texture_item_minigun  = Asset->Load<Texture>("textures/SpaceGame/circle_temp.png");

	A_material_texture_cam          = REF<Material>(Asset->Load<Shader>("shaders/texture_cam.shader"));
	A_material_texture_cam_particle = REF<Material>(Asset->Load<Shader>("shaders/particle/texture_cam.shader"));

	A_mesh_star            = iw::ScreenQuad().MakeCopy();
	A_mesh_ui_background   = iw::ScreenQuad().MakeInstance();
	A_mesh_ui_playerHealth = iw::ScreenQuad().MakeInstance();

#define CHECK_LOAD(x) if(!x) { LOG_ERROR << "Failed to load " << #x; return 100; }
	
	CHECK_LOAD(A_texture_enemy1        )
	CHECK_LOAD(A_texture_enemy1        )
	CHECK_LOAD(A_texture_player		   )
	CHECK_LOAD(A_texture_star 		   )
	CHECK_LOAD(A_texture_asteroid	   )
	CHECK_LOAD(A_texture_ui_cursor	   )
	CHECK_LOAD(A_texture_ui_background )
	CHECK_LOAD(A_texture_item_health   )
	CHECK_LOAD(A_texture_item_energy   )
	CHECK_LOAD(A_texture_item_minigun  )

	CHECK_LOAD(A_material_texture_cam         ->Shader)
	CHECK_LOAD(A_material_texture_cam_particle->Shader)

	A_texture_star         ->m_filter = NEAREST;
	A_texture_asteroid     ->m_filter = NEAREST;
	A_texture_player       ->m_filter = NEAREST;
	A_texture_enemy1       ->m_filter = NEAREST;
	A_texture_ui_cursor    ->m_filter = NEAREST;
	A_texture_ui_background->m_filter = NEAREST;
	A_texture_item_health  ->m_filter = NEAREST;
	A_texture_item_energy  ->m_filter = NEAREST;
	A_texture_item_minigun ->m_filter = NEAREST;

	
	A_texture_star         ->m_wrap = EDGE;
	A_texture_asteroid     ->m_wrap = EDGE;
	A_texture_player       ->m_wrap = EDGE;
	A_texture_enemy1       ->m_wrap = EDGE;
	A_texture_ui_cursor    ->m_wrap = EDGE;
	A_texture_ui_background->m_wrap = EDGE;
	A_texture_item_health  ->m_wrap = EDGE;
	A_texture_item_energy  ->m_wrap = EDGE;
	A_texture_item_minigun ->m_wrap = EDGE;

	{
		Renderer->InitShader(A_material_texture_cam->Shader, CAMERA);
		A_material_texture_cam->Set("useAlpha",    1);
		A_material_texture_cam->Set("alphaThresh", 0.9f);
	}

	{
		Renderer->InitShader(A_material_texture_cam_particle->Shader, CAMERA);
	}
	
	{
		A_mesh_ui_background.Material = A_material_texture_cam->MakeInstance();
		A_mesh_ui_background.Material->SetTexture("texture", A_texture_ui_background);
	}

	{
		ref<Texture> ui_player_texture = ref<Texture>(A_texture_player);
		ui_player_texture->SetFilter(iw::NEAREST);
		ui_player_texture->CreateColors();

		A_mesh_ui_playerHealth.Material = A_material_texture_cam->MakeInstance();
		A_mesh_ui_playerHealth.Material->SetTexture("texture", ui_player_texture);
	}

	{
		A_mesh_star.Material = A_material_texture_cam_particle;
		A_mesh_star.Material->SetTexture("texture", A_texture_star);
		A_mesh_star.Material->Set("useAlpha", 1);
	}

	return 0;
}
