#pragma once

// this should be a system that can load assets from a file
// right now this is hardcoded to Regolith but I needed some assets
// for the console and was lazy so moves this file into the engine !temp!

#include "iw/engine/Core.h"
#include "iw/asset/AssetManager.h"
#include "iw/graphics/Renderer.h"
#include "iw/graphics/Material.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Font.h"

#define rTex iw::ref<iw::Texture>
#define rMat iw::ref<iw::Material>
#define rFont iw::ref<iw::Font>

inline rTex A_texture_player;

inline rTex A_texture_enemy_fighter;
inline rTex A_texture_enemy_bomb;
inline rTex A_texture_enemy_station;
inline rTex A_texture_enemy_base;

inline rTex A_texture_background;
inline rTex A_texture_asteroid_mid_1;
inline rTex A_texture_asteroid_mid_2;
inline rTex A_texture_asteroid_mid_3;
inline rTex A_texture_item_health;
inline rTex A_texture_item_energy;
inline rTex A_texture_item_minigun;
inline rTex A_texture_item_boltz;
inline rTex A_texture_item_wattz;
inline rTex A_texture_item_coreShard;
inline rTex A_texture_ui_background;
inline rTex A_texture_ui_playerBorder;
inline rTex A_texture_ui_cursor;
inline rTex A_texture_post_game_background;

inline rFont A_font_cambria;

//inline rMat A_material_table_fade;

inline rMat A_material_texture_cam;
inline rMat A_material_font_cam;

inline rMat A_material_debug_wireframe;

inline iw::Mesh A_mesh_background;

inline iw::Mesh A_mesh_ui_background;
inline iw::Mesh A_mesh_ui_playerHealth;
inline iw::Mesh A_mesh_ui_playerBorder;
inline iw::Mesh A_mesh_ui_text_ammo;
inline iw::Mesh A_mesh_ui_text_score;
inline iw::Mesh A_mesh_ui_text_gameOver;
inline iw::Mesh A_mesh_ui_text_debug_version;

inline iw::Mesh A_mesh_menu_pause;
inline iw::Mesh A_mesh_menu_background;
inline iw::Mesh A_mesh_highscore_menu_background;

inline iw::Mesh A_mesh_extern_sand_game_layer;
inline iw::Mesh A_mesh_extern_ui_sand_tank_layer;

#undef rTex
#undef rMat

int LoadAssets(
	iw::AssetManager* Asset,
	iw::Renderer* Renderer);

//inline rTex A_texture_asteroid;
//inline rTex A_texture_star; // should be a texture atlas, animate star twinkle
//inline rMat A_material_texture_cam_particle;
//inline rMat A_material_debug_wireframe;
//inline iw::Mesh A_mesh_star;
//inline iw::Mesh A_mesh_ui_cursor;
