#include "Layers/Menu_Layer.h"

struct Menu_Title_Layer : Menu_Layer2
{
	iw::Entity ball;
	iw::Entity axis;
	iw::Entity title;
	iw::Entity stars;
	iw::ref<iw::RenderTarget> bg;

	Menu_Title_Layer()
		: Menu_Layer2("Menu Title")
	{}

	int Initialize() override;
	void UI() override;
};
//https://github.com/blender/blender/blob/master/source/blender/gpu/shaders/material/gpu_shader_material_tex_noise.glsl
