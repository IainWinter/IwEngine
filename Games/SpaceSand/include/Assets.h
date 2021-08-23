#pragma once

#include "iw/asset/AssetManager.h"
#include "iw/graphics/Renderer.h"
#include "iw/graphics/TextureAtlas.h"
#include "iw/graphics/Material.h"
#include "iw/graphics/Mesh.h"

inline iw::ref<iw::Texture> A_texture_player;
inline iw::ref<iw::Texture> A_texture_enemy1;
inline iw::ref<iw::Texture> A_texture_asteroid;

inline iw::ref<iw::Texture> A_texture_star; // should be a texture atlas, animate star twinkle

inline iw::ref<iw::Texture> A_texture_ui_background;
inline iw::ref<iw::Texture> A_texture_ui_cursor;

inline iw::ref<iw::Material> A_material_texture_cam;
inline iw::ref<iw::Material> A_material_texture_cam_particle;

inline iw::Mesh A_mesh_star;

inline iw::Mesh A_mesh_ui_background;
inline iw::Mesh A_mesh_ui_playerHealth;

int LoadAssets(
	iw::AssetManager* Asset,
	iw::Renderer* Renderer);
