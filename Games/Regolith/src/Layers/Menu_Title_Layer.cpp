#include "Layers/Menu_Title_Layer.h"
#include "iw/engine/Systems/Camera/EditorCameraControllerSystem.h"
#include "iw/engine/Systems/SpaceInspectorSystem.h"
#include "glm/gtc/random.hpp"
#include "iw/math/spline.h"
#include <cmath>

int Menu_Title_Layer::Initialize()
{
	RegisterImage("skybox_space.png");

	gameSettings.Add("VSync",      new VSyncSetting());
	gameSettings.Add("Fullscreen", new DisplaySetting(Window));
	gameSettings.Add("Music",      new AudioSetting(Audio, Audio->GetHandle("vca:/music")));
	gameSettings.Add("Game",       new AudioSetting(Audio, Audio->GetHandle("vca:/effects")));
	gameSettings.Init();

	//gameSettings.Add("Move Up", new ButtonMapSetting(Input, Input->GetContext("game"), "+up", iw::InputName::W));

	iw::MeshDescription desc;
	desc.DescribeBuffer(iw::bName::POSITION,  iw::MakeLayout<float>(3));
	desc.DescribeBuffer(iw::bName::NORMAL,    iw::MakeLayout<float>(3));
	desc.DescribeBuffer(iw::bName::TANGENT,   iw::MakeLayout<float>(3));
	desc.DescribeBuffer(iw::bName::BITANGENT, iw::MakeLayout<float>(3));
	desc.DescribeBuffer(iw::bName::UV,        iw::MakeLayout<float>(2));

	iw::Mesh mesh = iw::MakeUvSphere(desc, 48, 48)->MakeInstance();
	mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/test/pbr_noise.shader"));
	Renderer->Now->InitShader(mesh.Material->Shader, iw::CAMERA | iw::LIGHTS | iw::SHADOWS);

	mesh.Material->Set("albedo", iw::Color(1, .26, 0));
	mesh.Material->Set("metallic", .73f);
	mesh.Material->Set("roughness", .87f);
	mesh.Material->Set("reflectance", 0.0f);
	mesh.Material->Set("ao", 0.0f);
	mesh.Data->GenTangents();

	ball = Space->CreateEntity<iw::Transform, iw::Mesh>();
	ball.Set<iw::Transform>(glm::vec3(1.15, -3.75, .7), glm::vec3(3.f));
	ball.Set<iw::Mesh>(mesh);

	MainScene->SetAmbiance(.03);
	MainScene->AddLight(new iw::PointLight());
	MainScene->PointLights().at(0)->SetPosition(glm::vec3(.77, .52, .8));
	
	//iw::EditorCameraControllerSystem* camController = PushSystem<iw::EditorCameraControllerSystem>();
	//camController->Initialize();

	iw::Camera* cam = new iw::CustomCamera();
	cam->Transform.Rotation = target_rot;
	cam->Transform.Position = target_pos;

	MainScene->SetMainCamera(cam);
	Renderer->Now->Device->SetClearColor(0, 0, 0, 1);

	ortho = iw::OrthographicCamera(4, 5, 0, 50);
	persp = iw::PerspectiveCamera(.67, 4/5.f);
	//persp.RecalculateProjection();
	//iw::ISystem* sis = PushSystem<iw::SpaceInspectorSystem>();
	//sis->Initialize();

	// axis guide
	/*
	glm::vec3 pos[6] =
	{
		glm::vec3(0, 0, 0), glm::vec3(1, 0, 0),
		glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)
	};
	glm::vec3 col[6] =
	{
		glm::vec3(1, 0, 0), glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0), glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 1), glm::vec3(0, 0, 1),
	};
	unsigned idx[6] =
	{
		0, 1, 2, 3, 4, 5
	};
	iw::MeshDescription axis_desc;
	axis_desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
	axis_desc.DescribeBuffer(iw::bName::COLOR,    iw::MakeLayout<float>(3));
	iw::MeshData* axis_data = new iw::MeshData(axis_desc);
	axis_data->SetBufferData(iw::bName::POSITION, 6, pos);
	axis_data->SetBufferData(iw::bName::COLOR,    6, col);
	axis_data->SetIndexData(6, idx);
	axis_data->SetTopology(iw::MeshTopology::LINES);

	iw::Mesh axis_mesh = axis_data->MakeInstance();
	axis_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/test/axis_color.shader"));
	Renderer->Now->InitShader(axis_mesh.Material->Shader, iw::CAMERA);

	axis = Space->CreateEntity<iw::Transform, iw::Mesh>();
	axis.Set<iw::Transform>(glm::vec3(.8, 1.2, 10), glm::vec3(1.f));
	axis.Set<iw::Mesh>(axis_mesh);
	*/

	// title

	iw::Mesh title_mesh = Asset->Load<iw::Model>("models/SpaceGame/title_regolith.glb")->GetMesh(0);
	title_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/texture_cam.shader"));
	title_mesh.Material->Set("color", iw::Color(1));
	Renderer->Now->InitShader(title_mesh.Material->Shader, iw::CAMERA);

	title = Space->CreateEntity<iw::Transform, iw::Mesh>();
	title.Set<iw::Transform>(glm::vec3(-1.88, 1.446, -5), glm::vec3(1.5f));
	title.Set<iw::Mesh>(title_mesh);
	
	// title highscores

	iw::Mesh title_hs_mesh = Asset->Load<iw::Model>("models/SpaceGame/title_highscores.glb")->GetMesh(0);
	title_hs_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/texture_cam.shader"));
	title_hs_mesh.Material->Set("color", iw::Color(1));
	Renderer->Now->InitShader(title_hs_mesh.Material->Shader, iw::CAMERA);

	title_hs = Space->CreateEntity<iw::Transform, iw::Mesh>();
	title_hs.Set<iw::Transform>(glm::vec3(-10, 1.85, 1.88), glm::vec3(.8f), glm::quat(sqrt(2) / 2, 0, sqrt(2) / 2, 0));
	title_hs.Set<iw::Mesh>(title_hs_mesh);

	// title settings

	iw::Mesh title_st_mesh = Asset->Load<iw::Model>("models/SpaceGame/title_settings.glb")->GetMesh(0);
	title_st_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/texture_cam.shader"));
	title_st_mesh.Material->Set("color", iw::Color(1));
	Renderer->Now->InitShader(title_st_mesh.Material->Shader, iw::CAMERA);

	title_st = Space->CreateEntity<iw::Transform, iw::Mesh>();
	title_st.Set<iw::Transform>(glm::vec3(5.15, -4.8, 2), glm::vec3(.8f), glm::quat(.98, 0, .2, 0));
	title_st.Set<iw::Mesh>(title_st_mesh);

	// title pause

	//iw::Mesh title_ps_mesh = Asset->Load<iw::Model>("models/SpaceGame/title_pause.glb")->GetMesh(0);
	//title_ps_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/texture_cam.shader"));
	//title_ps_mesh.Material->Set("color", iw::Color(1));
	//Renderer->Now->InitShader(title_ps_mesh.Material->Shader, iw::CAMERA);

	//title_ps = Space->CreateEntity<iw::Transform, iw::Mesh>();
	//title_ps.Set<iw::Transform>(glm::vec3(0, 10, 3), glm::vec3(.8f), glm::quat(1, 0, 0, 0));
	//title_ps.Set<iw::Mesh>(title_ps_mesh);

	//stars

	glm::vec3 star_pos[1] =
	{
		glm::vec3(0, 0, 0)
	};
	unsigned star_idx[1] =
	{
		0
	};
	iw::MeshDescription star_desc;
	star_desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
	iw::MeshData* star_data = new iw::MeshData(star_desc);
	star_data->SetBufferData(iw::bName::POSITION, 1, star_pos);
	star_data->SetIndexData(1, star_idx);
	star_data->SetTopology(iw::MeshTopology::POINTS);

	iw::Mesh star_mesh = star_data->MakeInstance();
	star_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/particle/color_cam.shader"));
	star_mesh.Material->Set("color", iw::Color(1));
	Renderer->Now->InitShader(star_mesh.Material->Shader, iw::CAMERA);

	iw::StaticPS star_ps;
	star_ps.SetParticleMesh(star_mesh);

	for (int i = 0; i < 10000; i++)
	{
		star_ps.SpawnParticle(iw::Transform(
			glm::sphericalRand(15.f),
			glm::vec3(.01f)
		));
	}

	stars = Space->CreateEntity<iw::Transform, iw::StaticPS>();
	stars.Set<iw::Transform>();
	iw::StaticPS* star_p = stars.Set<iw::StaticPS>(star_ps);

	star_p->Update();
	star_p->UpdateParticleMesh();

	// stars as a cubemap

	// smoke

	glm::vec3 smoke_pos[4] =
	{
		glm::vec3(0, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 1, 0),
		glm::vec3(0, 1, 0)
	};
	glm::vec2 smoke_uv[4] =
	{
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1)
	};
	unsigned smoke_idx[6] =
	{
		0, 1, 2,
		0, 2, 3

	};
	iw::MeshDescription smoke_desc;
	smoke_desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
	smoke_desc.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));
	iw::MeshData* smoke_data = new iw::MeshData(smoke_desc);
	smoke_data->SetBufferData(iw::bName::POSITION, 4, smoke_pos);
	smoke_data->SetBufferData(iw::bName::UV,       4, smoke_uv);
	smoke_data->SetIndexData(6, smoke_idx);
	smoke_data->SetTopology(iw::MeshTopology::TRIANGLES);

	iw::Mesh smoke_mesh = smoke_data->MakeInstance();
	smoke_mesh.Material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/particle/texture_cam.shader"));
	smoke_mesh.Material->SetTexture("texture", LoadTexture("circle_temp.png"));
	Renderer->Now->InitShader(smoke_mesh.Material->Shader, iw::CAMERA);

	iw::StaticPS smoke_ps;
	smoke_ps.SetParticleMesh(smoke_mesh);

	smoke = Space->CreateEntity<iw::Transform, iw::StaticPS>();
	smoke.Set<iw::Transform>();
	smoke.Set<iw::StaticPS>(smoke_ps);

	// render target to put the scene into imgui

	bg = REF<iw::RenderTarget>();
	bg->AddTexture(REF<iw::Texture>(Renderer->Width(), Renderer->Height()));
	bg->AddTexture(REF<iw::Texture>(Renderer->Width(), Renderer->Height(), iw::TEX_2D, iw::DEPTH));
	bg->Initialize(Renderer->Device);
	RegisterImage("bg", (void*)bg->Tex(0)->Handle()->Id());

	highscoreParts.LoadTopScore();

	drawMenubg = true;

	return 0;
}

float easeInOut(float x)
{
	return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
}

float easeIn(float x)
{
	return x * x * x * x;
}

void Menu_Title_Layer::UI()
{
	highscoreParts.UpdateBG(this);
	upgradeParts.UpdateBG(this);
	decalParts.UpdateBG(this);

	// testing camera moves, these dont really matter and it seems like if I can get a cool skybox and render the sun, then these will
	// fall into place

	//ImGui::SetCursorPos(ImVec2(100, 100));
	//ImGui::SliderFloat("fov", &persp.Fov, 0, iw::Pi);
	//ImGui::SetCursorPosX(100);
	//ImGui::SliderFloat("prs", &target_pers, 0, 1);
	//ImGui::SetCursorPosX(100);
	//ImGui::SliderFloat3("pos", (float*)&target_pos, -100, 100);
	//persp.RecalculateProjection();

	//iw::StaticPS* smoke_p = smoke.Find<iw::StaticPS>();

	//glm::vec3 p1 = glm::vec3(1.15, -3.75, .7) + glm::normalize(glm::vec3(-.5, 1, 1)) * 3.f;

	//iw::spline3 spline1;
	//spline1.A = p1;
	//spline1.B = glm::vec3(p1.x,     .2, p1.z);
	//spline1.C = glm::vec3(p1.x - 1, .4, p1.z);

	//iw::spline3 spline2;
	//spline2.A = spline1.C;
	//spline2.B = glm::vec3(-3, spline1.C.y, p1.z);
	//spline2.C = glm::vec3( 3,  1.2, p1.z);

	//float time = iw::TotalTime() * .9 - 1;

	//if (time >= 0 && time < 2)
	//{
	//	iw::Transform tr;
	//	tr.Scale = glm::vec3(.01);
	//	tr.Position = time < 1 ? spline1.F(time) : spline2.F(time - 1);

	//	smoke_p->SpawnParticle(tr);
	//	smoke_p->Update();
	//	smoke_p->UpdateParticleMesh();
	//}

	iw::Camera* cam = MainScene->MainCamera();

	glm::vec3& pos = cam->Transform.Position;
	glm::quat& rot = cam->Transform.Rotation;

	if (target_menu != last_menu)
	{
		last_menu = target_menu;
		last_pos = pos;
		last_rot = rot;
		last_pers = pers;

		t  = 0;
		t1 = 0;
	}

	if (t >= 1) t = 1;
	else        t += iw::DeltaTime();

	if (t1 >= 1) t1 = 1;
	else         t1 += iw::DeltaTime() * 3;

	pos = iw::  lerp(last_pos, target_pos, easeInOut(t));
	rot = glm::slerp(last_rot, target_rot, easeInOut(t));

	if (normalize)
	{
		pos = glm::normalize(pos) * 10.f;
	}

	pers = iw::lerp(last_pers, target_pers, easeInOut(t1));
	cam->Projection = iw::lerp(ortho.Projection, persp.Projection, pers);
	//title_ps.Find<iw::Transform>()->Position = iw::lerp(glm::vec3(0, 10, 2), glm::vec3(3.7, 7.7, -3), pers);

	float menuOffset  = iw::lerp(1.f, 0.f, easeInOut(t1)) * 50;
	float menuOpacity = iw::lerp(0.f, 1.f, easeIn   (t1));

	// end temp

	ImGui::PushFont(iwFont("Quicksand_40"));

	ImGui::PushStyleColor(ImGuiCol_FrameBg,        ImVec4(1, 1, 1, 0.2 * menuOpacity));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  ImVec4(1, 1, 1, 0.1 * menuOpacity));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1, 1, 1, 0.3 * menuOpacity));

	ImGui::PushStyleColor(ImGuiCol_CheckMark,        ImVec4(1, 1, 1, 1.0 * menuOpacity));
	ImGui::PushStyleColor(ImGuiCol_SliderGrab,       ImVec4(1, 1, 1, 0.2 * menuOpacity));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1, 1, 1, 0.4 * menuOpacity));

	ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(1, 1, 1, 1.0 * menuOpacity));
	ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0, 0, 0, 0.0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.2 * menuOpacity));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(1, 1, 1, 0.1 * menuOpacity));

	ImGui::SetNextWindowFocus();
	ImGui::SetNextWindowPos (ImVec2(bg_x, bg_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h));
	ImGui::Begin("Main Menu Layer", nullptr, commonFlagsFocus);
	
	fade = iw::lerp(fade, target_fade, iw::DeltaTime() * 20.f);

	if (fade > 0.001f)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(bg_x, bg_y),
			ImVec2(bg_x + bg_w, bg_y + bg_h), iw::Color::From255(31, 32, 38, 255 * fade).to32()
		);
	}

	if (fade_exit > 0.001f)
	{
		ImGui::GetForegroundDrawList()->AddRectFilled(
			ImVec2(bg_x, bg_y),
			ImVec2(bg_x + bg_w, bg_y + bg_h), iw::Color(0, 0, 0, fade_exit).to32()
		);
	}

	// tag + fps

	__fps = iw::clamp(iw::lerp(__fps, 1.f / iw::RawDeltaTime(), iw::RawDeltaTime()), 0.f, 10000.f);
	ImGui::PushFont(iwFont("Quicksand_24"));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, .5));
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Text("indev v.05 fps: %.0f", __fps);
	ImGui::PopStyleColor();
	ImGui::PopFont();

	// draw buttons

	float x_padding = bg_w / 6;
	float x = x_padding - menuOffset;

	if (last_menu == MenuTarget::DEFAULT)
	{
		ImGui::SetCursorPos(ImVec2(x, bg_h / 2));
		if (Button("Play"))
		{
			SetViewGame(); // do little play animation
			Console->QueueCommand("set-state game 1");
		}

		ImGui::SetCursorPosX(x);
		if (Button("Highscores"))
		{
			SetViewHighscores();
			PushBackState(MenuTarget::DEFAULT);
		}

		ImGui::SetCursorPosX(x);
		if (Button("Settings"))  // should rename to 'System'
		{
			SetViewSettings();
			PushBackState(MenuTarget::DEFAULT);
		}
	}

	if (last_menu == MenuTarget::HIGHSCORES)
	{
		highscoreParts.ScoreTable(
			bg_x + x,
			bg_y + bg_h / 3, 
			bg_w - x_padding * 2,
			bg_h / 2
		);
	}

	if (last_menu == MenuTarget::SETTINGS)
	{
		float y = IsFromPause()
				? bg_h / 3 
				: bg_h / 2;

		ImGui::PushFont(iwFont("Quicksand_30"));
		{
			gameSettings.Draw(x, y, bg_w / 2);
			
			if (!IsFromPause()) // this seems sloppy
			{
				ImGui::SetCursorPosX(x);
				if (Button("Controls"))
				{
					target_menu = MenuTarget::SETTINGS_CONTROLS;
					PushBackState(MenuTarget::SETTINGS);
				}
			}

			ImGui::SetCursorPosX(x);
			if (ConfirmButton("Force Exit"))
			{
				Console->QueueCommand("exit");
			}
		}
		ImGui::PopFont();
	}

	if (last_menu == MenuTarget::SETTINGS_CONTROLS)
	{
		ImGui::SetCursorPosY(bg_h / 3);
		ImGui::SetCursorPosX(x);
		ImGui::Text("Button remapping is wip");

		ImGui::SetCursorPosX(x);
		ImGui::Text("W, A, S, D: Movement");

		ImGui::SetCursorPosX(x);
		ImGui::Text("L/R Mouse: Fire / Special Fire");

		//ImGui::SetCursorPosX(x);
		//ImGui::Text("E/F: Ability 1/ Ability 2");
	}

	if (last_menu == MenuTarget::PAUSE)
	{
		ImGui::SetCursorPosY(bg_h / 3);
		ImGui::SetCursorPosX(x);
		if (Button("Settings"))
		{
			target_menu = MenuTarget::SETTINGS;
			PushBackState(MenuTarget::PAUSE);
		}

		ImGui::SetCursorPosX(x);
		if (Button("Controls"))
		{
			target_menu = MenuTarget::SETTINGS_CONTROLS;
			PushBackState(MenuTarget::PAUSE);
		}

		ImGui::SetCursorPosX(x);
		if (ConfirmButton("Save and Quit"))
		{
			// impl, testing quit button
		}

		ImGui::SetCursorPosY(bg_h * .8 - padding_1);
		ImGui::SetCursorPosX(x);
		if (Button("Resume")) Console->QueueCommand("escape");
	}

	// back buttons for sub menus

	if (   last_menu == MenuTarget::HIGHSCORES
		|| last_menu == MenuTarget::SETTINGS
		|| last_menu == MenuTarget::SETTINGS_CONTROLS)
	{
		float y = IsFromPause()
				? bg_h * .8 - padding_1
				: bg_h      - padding_1;

		ImGui::SetCursorPos(ImVec2(x, y));

		if (gameSettings.HasChanged)
		{
			if (Button("Reset")) gameSettings.Reset();
			ImGui::SameLine();
			if (Button("Apply")) gameSettings.Apply();
		}

		else if (Button("Back")) GoBack();
	}

	if (last_menu == MenuTarget::POST_GAME)
	{
		float img_wh = bg_w / 2;
		float img_xy = (bg_w - img_wh) / 2;

		ImGui::PushFont(iwFont("Quicksand_120"));
		ImVec2 size = ImGui::CalcTextSize("GAME OVER");
		
		float title_x = bg_w / 2 - size.x / 2;
		float title_y = img_xy / 2 - size.y / 2;

		ImGui::SetCursorPos(ImVec2(title_x, title_y));
		ImGui::Text("GAME OVER");
		ImGui::PopFont();

		// decals

		decalParts.lines.at(0).x  = img_xy;
		decalParts.lines.at(0).y  = img_xy;
		decalParts.lines.at(0).x2 = img_xy;
		decalParts.lines.at(0).y2 = bg_h;

		decalParts.lines.at(1).x  = 0;
		decalParts.lines.at(1).y  = bg_h - padding_1;
		decalParts.lines.at(1).x2 = bg_w;
		decalParts.lines.at(1).y2 = bg_h - padding_1;

		decalParts.DrawLines();

		// Death movie

		glm::vec2 uv0 = glm::vec2(0, 0);
		glm::vec2 uv1 = glm::vec2(1, 1);
		uv0 = deathMovie.MapUv(deathMovieFrame, uv0);
		uv1 = deathMovie.MapUv(deathMovieFrame, uv1);
		ImGui::SetCursorPos(ImVec2(img_xy, img_xy));
		ImGui::Image(
			Image("deathMovie"), 
			ImVec2(img_wh, img_wh), 
			ImVec2(uv0.x, uv0.y), 
			ImVec2(uv1.x, uv1.y), 
			ImVec4(1, 1, 1, 1), 
			decalParts.lines.at(0).color
		);

		// debug frame count
		// ImGui::Text("Frame %d/%d, step %.2f", deathMovieFrame, deathMovie.TileCount(), deathMovieFrameTimer.GetTime("step"));

		deathMovieFrameTimer.Tick();
		if (deathMovieFrameTimer.Can("step"))
		{
			deathMovieFrame = (deathMovieFrame + 1) % deathMovie.TileCount();
		}

		// stats

		auto printStat = [](const std::string& title, const std::string& value)
		{
			ImGui::TableNextCell();
			ImGui::Text(title.c_str());
			ImGui::TableNextCell();
			RightAlign(value);
			ImGui::Text(value.c_str());
		};

		ImGui::SetCursorPos(ImVec2(img_xy + padding_01, img_xy + img_wh));
		ImGui::BeginTable("stat-table", 2, 0, ImVec2(img_wh, img_wh));

			// Score

			std::stringstream ss;

			ss = {}; ss << m_finalScore << "r";
			printStat("Regolith", ss.str());

			// Progress

			ss = {}; ss << m_finalProgress << "%"; // todo: percent isnt in font????
			printStat("Progress", ss.str());

			ImGui::PushFont(iwFont("Quicksand_24"));

			printStat("Time alive", "3:10 min");
			printStat("Killed by", "Enemy fighter");
			printStat("Accuracy", "33.4%");
			printStat("Favourite weapon", "Minigun");

			ImGui::PopFont();

		ImGui::EndTable();

		// buttons

		ImGui::SetCursorPosY(bg_h - padding_1 + padding_01);
		ImGui::SetCursorPosX(img_xy + padding_01);
		if (Button("Continue"))
		{
			SetViewUpgrade();
			decalParts.lines.emplace_back(Decals_MenuParts::Line { 0, 0, 0, 0, 1, decalParts.lines.at(0).color });
		}
	}

	if (last_menu == MenuTarget::UPGRADE)
	{
		float img_wh = bg_w / 2;
		float img_xy = (bg_w - img_wh) / 2;

		// title
		
		ImGui::PushFont(iwFont("Quicksand_120"));
		ImVec2 size = ImGui::CalcTextSize("UPGRADES");
		
		float title_x = bg_w / 2 - size.x / 2;
		float title_y = img_xy / 2 - size.y / 2;

		ImGui::SetCursorPos(ImVec2(title_x, title_y));
		ImGui::Text("UPGRADES");
		ImGui::PopFont();

		// decals

		if (m_decalPostGameTime > 1.f)
		{
			decalParts.lines.at(0).y  = 0;
			decalParts.lines.at(0).x  = padding_1;
			decalParts.lines.at(0).x2 = padding_1;

			decalParts.lines.at(2).x2 = bg_w;
		}

		else
		{
			m_decalPostGameTime = iw::lerp(m_decalPostGameTime, 1.f, iw::DeltaTime() * 10);

			decalParts.lines.at(0).y  = iw::lerp(img_xy,       0.f, m_decalPostGameTime);
			decalParts.lines.at(0).x  = iw::lerp(img_xy, padding_1, m_decalPostGameTime);
			decalParts.lines.at(0).x2 = iw::lerp(img_xy, padding_1, m_decalPostGameTime);

			decalParts.lines.at(2).x2 = iw::lerp(0.f, bg_w, m_decalPostGameTime);;
		}

		decalParts.lines.at(0).y2 = bg_h;

		decalParts.lines.at(1).x  = 0;
		decalParts.lines.at(1).y  = bg_h - padding_1;
		decalParts.lines.at(1).x2 = bg_w;
		decalParts.lines.at(1).y2 = bg_h - padding_1;

		float tooltip_y = bg_h / 3;

		decalParts.lines.at(2).x = 0;
		decalParts.lines.at(2).y  = tooltip_y;
		decalParts.lines.at(2).y2 = tooltip_y;

		decalParts.DrawLines();

		upgradeParts.UpgradeTable(
			padding_1 + padding_01, 
			bg_h * .5f/* - padding_1 - padding_01*/, 
			bg_w - padding_1 * 2 - padding_01,
			bg_h * .5f
		);

		upgradeParts.Tooltip(
			padding_1 + padding_01, 
			tooltip_y + padding_01
		);

		// buttons

		ImGui::SetCursorPosY(bg_h - padding_1 + padding_01);
		ImGui::SetCursorPosX(padding_1 + padding_01);
		if (Button("Continue"))
		{
			SetViewDefault();
			decalParts.lines.pop_back(); // state hack
		}
	}

	ImGui::End();
	ImGui::PopStyleColor(10);
	ImGui::PopFont();

	iw::Transform* strans = stars.Find<iw::Transform>();
	strans->Position = cam->Transform.Position / 10.f;

	float roll = glm::length(cam->Transform.Position);

	// could save some frames by only redrawing when moving once game has started

	bg->Resize(bg_w, bg_h);
	Renderer->BeginScene(MainScene, bg, true);
	{
		//Renderer->DrawMesh(stars   .Find<iw::Transform>(), &stars  .Find<iw::StaticPS>()->GetParticleMesh());
		
		if (drawMenubg)
		{
			Renderer->DrawMesh(ball    .Find<iw::Transform>(), ball    .Find<iw::Mesh>());
			Renderer->DrawMesh(smoke   .Find<iw::Transform>(), &smoke  .Find<iw::StaticPS>()->GetParticleMesh());
			Renderer->DrawMesh(title   .Find<iw::Transform>(), title   .Find<iw::Mesh>());
			Renderer->DrawMesh(title_hs.Find<iw::Transform>(), title_hs.Find<iw::Mesh>());
			Renderer->DrawMesh(title_st.Find<iw::Transform>(), title_st.Find<iw::Mesh>());
			//Renderer->DrawMesh(title_ps.Find<iw::Transform>(), title_ps.Find<iw::Mesh>());
			//Renderer->DrawMesh(axis .Find<iw::Transform>(), axis  .Find<iw::Mesh>());
		}
	}
	Renderer->EndScene();
}
