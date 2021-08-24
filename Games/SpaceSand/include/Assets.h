#pragma once

#include "iw/asset/AssetManager.h"
#include "iw/graphics/Renderer.h"
#include "iw/graphics/TextureAtlas.h"
#include "iw/graphics/Material.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Font.h"

#define rTex iw::ref<iw::Texture>
#define rMat iw::ref<iw::Material>
#define rFont iw::ref<iw::Font>

inline rTex A_texture_player;
inline rTex A_texture_enemy1;
inline rTex A_texture_asteroid;

inline rTex A_texture_star; // should be a texture atlas, animate star twinkle

inline rTex A_texture_ui_background;
inline rTex A_texture_ui_cursor;

inline rTex A_texture_item_health;
inline rTex A_texture_item_energy;
inline rTex A_texture_item_minigun;


inline rMat A_material_texture_cam;
inline rMat A_material_texture_cam_particle;

inline rTex A_texture_font_arial;
inline rMat A_material_font_cam;
inline rFont A_font_arial;

inline rMat A_material_debug_wireframe;

inline iw::Mesh A_mesh_star;

inline iw::Mesh A_mesh_ui_background;
inline iw::Mesh A_mesh_ui_playerHealth;
inline iw::Mesh A_mesh_ui_text_ammo;

#undef rTex
#undef rMat

int LoadAssets(
	iw::AssetManager* Asset,
	iw::Renderer* Renderer);