//#include "iw/engine/Layers/Console_Layer.h"
//
//namespace iw {
//namespace Engine
//{
//	int Console_Layer::Initialize()
//	{
//		// Assets, maybe engine should have a asset store ontop of
//		// the asset manager that loads a set of things from a file...
//
//		auto tex_mat  = REF<Material>(Asset->Load<Shader>("shaders/texture_cam.shader"));
//		auto font_mat = REF<Material>(Asset->Load<Shader>("shaders/font.shader"));
//		
//		font_mat->Set("texture");
//		font_mat->Set("color", Color(1));
//
//		auto tex_mat_bg = tex_mat->MakeInstance();
//		tex_mat_bg->Set("color", Color(0, 0, 0, .25));
//
//		auto tex_mat_textbg = tex_mat->MakeInstance();
//		tex_mat_textbg->Set("color", Color(.2, .2, .2, .25));
//
//		m_screen = new UI_Screen();
//
//		iw::Mesh bg = iw::ScreenQuad().MakeInstance();
//		bg.Material = tex_mat;
//
//		m_bg   = m_screen->CreateElement();
//		m_text = m_screen->CreateElement();
//		m_log  = m_screen->CreateElement();
//
//		return 0;
//	}
//}
//}
